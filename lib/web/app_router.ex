defmodule Web.AppRouter do
  @moduledoc """
  Highest level router for the web server
  """
  use Plug.Router

  if Mix.env == :dev do
    use Plug.Debugger
  end

  plug :match
  plug :dispatch

  forward "/api", to: Web.Handlers.Api
  
  # files at the root of the build directory
  get "/favicon.ico", do: send_file(conn, 200, Web.Ui.StaticAssets.favicon)
  get "/asset-manifest.json", do: send_file(conn, 200, Web.Ui.StaticAssets.asset_manifest)
  get "/manifest.json", do: send_file(conn, 200, Web.Ui.StaticAssets.manifest)
  get "/service-worker.js", do: send_file(conn, 200, Web.Ui.StaticAssets.service_worker)

  # any static resources
  forward "/static", to: Web.Ui.StaticAssets

  forward "/", to: Web.Handlers.Root

  # anything else, just return the index.html
  #get "*_anything", do: send_file(conn, 200, Ui.StaticAssets.index)
end
