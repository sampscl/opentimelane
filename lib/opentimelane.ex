defmodule Opentimelane.Application do
  @moduledoc """
  Application entrypoint
  """
  use Application

  def start(_type, _args) do
    # List all child processes to be supervised
    # See https://hexdocs.pm/elixir/Supervisor.html
    # for other strategies and supported options

    result = children() |> Supervisor.start_link([strategy: :one_for_all, name: Opentimelane.Application])

    result
  end

  def children do
    [
      {FifoInput.Sup, :ok},
      {EventLogger, :ok},
    ]
  end
end
