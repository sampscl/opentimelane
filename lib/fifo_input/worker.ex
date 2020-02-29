# fifo_path
# FiFoInput.Worker

defmodule FiFoInput.Worker do
  use GenServer
  ##############################
  # API
  ##############################

  def start_link(fifo_path) do
    GenServer.start_link(__MODULE__, [fifo_path])
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
  def init([fifo_path]) do
    {:ok, %State{}}
  end

  ##############################
  # Internal Calls
  ##############################

end
