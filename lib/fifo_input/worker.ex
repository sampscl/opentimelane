# fifo_path
# FiFoInput.Worker

defmodule FiFoInput.Worker do
  @moduledoc """
  Fifo input worker. Reads line-buffered data from a fifo and produces
  `timelane_event`s from it.

  The line is expected to be JSON of the form:
    `{
      "app": "APP",
      "module": "MODULE",
      "time": "%Y-%m-%d-%H-%M-%S.%f",
      "metadata": {}
    }`
  except on a single line. The time accepts 6 digits after the decimas seconds
  and should be in UTC (e.g. no timezone).
  """
  use GenServer
  use LoggerUtils
  import ShorterMaps

  @strftime_fmt "%Y-%m-%d-%H-%M-%S.%f"

  ##############################
  # API
  ##############################

  def start_link(fifo_path) do
    GenServer.start_link(__MODULE__, [fifo_path])
  end

  defmodule State do
    @doc false
    defstruct [
      fifo_path: nil, # path to the fifo (stored here for debugability)
      fifo: nil, # the fifo
    ]
  end

  ##############################
  # GenServer Callbacks
  ##############################

  @impl true
  def init([fifo_path]) do
    LoggerUtils.info("starting #{fifo_path}")

    # remove and re-create fifo
    System.cmd("rm", ["-f", fifo_path])
    {_, 0} = System.cmd("mkfifo", [fifo_path])

    # make sure fifo is cleaned up
    Ghoul.summon(fifo_path, on_death: fn(fifo_path, reason, _ghoul_state) ->
      LoggerUtils.warn("Owner died (#{inspect(reason)}), killing fifo #{fifo_path}")
      System.cmd("rm", ["-f", fifo_path])
    end)

    fifo = File.open!(fifo_path, [:read])

    send(self(), :next_work_item)

    {:ok, %State{fifo_path: fifo_path, fifo: fifo}}
  end

  @impl true
  def handle_info(:next_work_item, ~M{fifo} = state) do
    case IO.read(fifo, :line) do
      :eof ->
        Process.send_after(self(), :next_work_item, 100)
        {:noreply, state}

      {:error, reason} ->
        LoggerUtils.error("IO.read() => {:error, #{inspect(reason)}}")
        {:stop, {:error, reason}}

      data ->
        {:noreply, do_handle_data(state, data)}
    end
  end

  ##############################
  # Internal Calls
  ##############################

  def do_handle_data(~M{fifo_path} = state, data) do
    LoggerUtils.debug("data => #{inspect(data, pretty: true)}")
    case Poison.decode!(data) do
      ~m{app, module, time, event, metadata} = tl_event ->
        case Timex.parse(time, @strftime_fmt, :strftime) do
          {:ok, dt} ->
            LoggerUtils.debug("tl_event => #{inspect(tl_event, pretty: true)}")
            PubSub.pub_timelane_event(~M{app, module, time: dt, event, metadata})

          bad ->
            LoggerUtils.info("Malformed time data on fifo #{fifo_path}: #{inspect(bad, pretty: true)}")
        end

      bad ->
        LoggerUtils.info("Malformed data on fifo #{fifo_path}: #{inspect(bad, pretty: true)}")
    end
    state
  end
end
