defmodule Log.Sup do
  @moduledoc """
  Supervisor for logging
  """
  use Supervisor

  def start_link(:ok) do
    Supervisor.start_link(__MODULE__, :ok, name: __MODULE__)
  end

  @impl true
  def init(:ok) do
    children = [
      {Log.Worker, [:ok]},
    ]
    Supervisor.init(children, strategy: :one_for_one)
  end
end
