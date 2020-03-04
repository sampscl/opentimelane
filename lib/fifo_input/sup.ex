defmodule FifoInput.Sup do
  @moduledoc """
  Supervisor for fifo input
  """
  use DynamicSupervisor
  use LoggerUtils

  def start_link(:ok) do
    DynamicSupervisor.start_link(__MODULE__, :ok, name: __MODULE__)
  end

  @doc """
  Start a fifo worker

  ## Parameters:
  - fifo_path: The fifo to read

  ## Returns
  - {:ok, pid} on success
  - {:error, reason} on failure
  """
  def start_worker(fifo_path) do
    DynamicSupervisor.start_child(__MODULE__, {FiFoInput.Worker, fifo_path})
  end


  ##############################
  # GenServer Callbacks
  ##############################

  def init(:ok) do
    LoggerUtils.info("starting")
    DynamicSupervisor.init(strategy: :one_for_one)
  end
end
