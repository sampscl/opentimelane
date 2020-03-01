# :ok
# EventLogger

defmodule EventLogger do
  use GenServer
  use LoggerUtils
  import ShorterMaps

  ##############################
  # API
  ##############################

  def start_link(:ok) do
    GenServer.start_link(__MODULE__, [:ok], [name: __MODULE__])
  end

  defmodule State do
    @doc false
    defstruct [
    ]
  end

  ##############################
  # GenServer Callbacks
  ##############################

  @impl true
  def init([:ok]) do
    LoggerUtils.info("starting")
    PubSub.sub_timelane_event()
    {:ok, %State{}}
  end

  @impl true
  def handle_info(%Flub.Message{data: ~M{%PubSub.TimelaneEvent} = event}, state) do
    LoggerUtils.info(inspect(event, pretty: true))
    {:noreply, state}
  end

  ##############################
  # Internal Calls
  ##############################

end
