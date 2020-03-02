# :ok
# Web.AssetServer

defmodule Web.AssetServer do
  @moduledoc """
  Provide access to web asset configuration data
  """
  use GenServer
  use LoggerUtils
  import ShorterMaps

  ##############################
  # API
  ##############################

  @spec web_root() :: String.t | {:error, any()}
  def web_root(), do: __MODULE__ |> GenServer.call(:web_root)

  @spec secure() :: boolean()
  def secure(), do: __MODULE__ |> GenServer.call(:secure)

  def start_link(:ok) do
    GenServer.start_link(__MODULE__, [:ok], [name: __MODULE__])
  end

  defmodule State do
    @moduledoc false
    defstruct [
      web_root: nil, # path to the root of the web assets
      secure: true,
    ]
  end

  ##############################
  # GenServer Callbacks
  ##############################

  def init([:ok]) do
    web_root = "#{:code.priv_dir(:opentimelane) |> Path.join("ui/build")}"
    secure = false
    {:ok, ~M{%State web_root, secure}}
  end

  def handle_call(:web_root, _from, ~M{web_root} = state) do
    {:reply, web_root, state}
  end

  def handle_call(:secure, _from, ~M{secure} = state) do
    {:reply, secure, state}
  end

  ##############################
  # Internal Calls
  ##############################
end
