# log_path
# Log.Worker

defmodule Log.Worker do
  @moduledoc """
  Log worker
  """
  use GenServer
  ##############################
  # API
  ##############################

  def start_link(log_path) do
    GenServer.start_link(__MODULE__, [log_path])
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
  def init([log_path]) do
    {:ok, %State{}}
  end

  ##############################
  # Internal Calls
  ##############################

end
