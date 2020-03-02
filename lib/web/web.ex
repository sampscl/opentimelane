defmodule Web do
  @moduledoc """
  Api for the web application
  """

  @spec certs_path() :: String.t
  @doc """
  Get path to dir where cert.pem and key.pem are stored. This function has no
  web-internal dependencies and can be called anytime the web dependencies are
  started.

  ## Return
  - String path to directory where cert.pem and key.pem are stored
  """
  def certs_path() do
    case Settings.get(:web, :certs_path) do
      :priv -> "#{:code.priv_dir(:web)}"
      path  -> path
    end
  end

  @spec web_root() :: String.t | {:error, any()}
  @doc """
  Get the full path to the directory containing the web app build. This depends
  on the web app being started.

  ## Return
  - String: Path to directory containing the web app build
  - {:error, reason}: there was a problem
  """
  def web_root(), do: Web.AssetServer.web_root()

  @spec secure() :: boolean()
  @doc """
  Get secure mode true or false

  ## Return
  - true: use secure mode (https)
  - false: use http
  """
  def secure(), do: Web.AssetServer.secure()
end
