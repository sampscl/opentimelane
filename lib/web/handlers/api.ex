defmodule Web.Handlers.Api do
  @moduledoc """
  Handle highest level api calls
  """
  alias Utils.Json
  alias Web.Utils.Mime
  use Plug.Router

  if Mix.env == :dev do
    plug Plug.Logger
  end
  plug :match
  plug :dispatch

  #
  # API
  #

  match _ do
    conn
    |> put_resp_content_type(Mime.json())
    |> send_resp(404, "")
  end

  #
  # Implementation
  #
end
