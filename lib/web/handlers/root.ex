defmodule Web.Handlers.Root do
  @moduledoc """
  Root handler
  """
  use Plug.Router
  alias Web.Utils.Mime

  if Mix.env == :dev do
    plug Plug.Logger
  end

  plug :match
  plug :dispatch

  get "/" do
    conn
    |> put_resp_content_type(Mime.html())
    |> send_file(200, Web.Ui.StaticAssets.index())
  end

  match _ do
    conn
    |> put_resp_content_type(Mime.plain_text())
    |> send_resp(404, "")
  end

end
