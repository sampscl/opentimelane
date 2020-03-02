# :ok
# Web.Websocket.Worker

defmodule Web.Websocket.Worker do
  @moduledoc """
  Manage pub sub publishing to websockets
  """
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
    @moduledoc false
    defstruct [
    ]
  end

  ##############################
  # GenServer Callbacks
  ##############################

  def init([:ok]) do
    LoggerUtils.info("starting")

    PubSub.sub_timelane_event()
    {:ok, %State{}}
  end

  def handle_info(~M{%Flub.Message data}, state) do
    Web.Handlers.Websocket.send_to_clients(data)
    {:noreply, state}
  end

  ##############################
  # Internal Calls
  ##############################

end
