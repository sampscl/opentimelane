defmodule Web.Handlers.Websocket do
  @moduledoc """
  Manage a web socket to a browser
  """
  @behaviour(:cowboy_websocket)
  use LoggerUtils
  import ShorterMaps

  def send_to_all(msg) do
    pkg = %{type: "all", body: msg}
    for pid <- :gproc.lookup_pids({:p, :l, __MODULE__}) do
      send(pid, pkg)
    end
  end

  def send_to_clients(~M{%PubSub.TimelaneEvent} = msg), do: send_msg("timelane_event", PubSub.timelane_event_chnl(), msg)
  def send_msg(type, channel, body) do
    LoggerUtils.debug("#{inspect(~M/type, channel, body/, pretty: true)}")
    pkg = ~M{type, body}
    for pid <- :gproc.lookup_pids({:p, :l, {__MODULE__, channel}}) do
      LoggerUtils.debug("#{inspect(~M/pid, pkg/, pretty: true)}")
      send(pid, pkg)
    end
  end

  defmodule State do
    @moduledoc false
    defstruct [
    ]
  end

  def init(req, _state)  do
    {:cowboy_websocket, req, ~M{%State}}
  end

  def websocket_init(_state) do
    :gproc.reg({:p, :l, __MODULE__})
    :gproc.reg({:p, :l, {__MODULE__, PubSub.timelane_event_chnl()}})
  end

  def websocket_handle(_frame, state) do
    {:ok, state}
  end

  def websocket_info(_msg, state) do
    {:ok, state}
  end

  def terminate(reason, _req, _state) do
    LoggerUtils.debug("terminating websocket (#{inspect(reason)})")
    :ok
  end

end
