# :ok
# Datastore

defmodule Datastore do
  use GenServer
  use ExternalState, persist: true, props: [
    datastore: %{}, # 
    oldest: nil, # DateTime.utc_now()
    newest: nil # DateTime.utc_now()
  ]

  ##############################
  # API
  ##############################

  @doc """
  Get the datastore.

  ## Returns
  - A map containing the datastore
  """
  def get do: get_ex_state().datastore

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
    init_ex_state()
    {:ok, %State{}}
  end

  ##############################
  # Internal Calls
  ##############################

end
