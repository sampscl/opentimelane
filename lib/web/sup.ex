defmodule Web.Sup do
  @moduledoc false

  use Supervisor
  use LoggerUtils

  #############
  # API
  #############

  def start_link(:ok) do
    Supervisor.start_link(__MODULE__, [:ok], name: __MODULE__)
  end

  ##############################
  # GenServer Callbacks
  ##############################

  def init([:ok]) do
    Supervisor.init(children(), strategy: :one_for_one)
  end

  ##############################
  # Internal
  ##############################

  def dispatch do
    [{:_,
      [
        # special handling in dispatch for the websocket,
        # since there isn't a plug for ws.
        {"/opentimelane_ws", Web.Handlers.Websocket, []},
        {:_, Plug.Cowboy.Handler, {Web.AppRouter, []}}
      ]
    }]
  end

  def children do
    LoggerUtils.info("Using http")
    [
      {Web.AssetServer, :ok},
      {Web.Websocket.Worker, :ok},
      {Plug.Cowboy, [
        scheme: :http,
        plug: Web.AppRouter,
        options: [
          port: 8080,
          otp_app: :opentimelane,
          dispatch: dispatch()
          ]
        ]
      },
    ]
  end
end
