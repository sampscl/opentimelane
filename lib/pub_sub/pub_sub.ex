defmodule PubSub do
  @moduledoc """
  PubSub module contains modules and functions related to pub/sub. Duh.
  """
  import PubSub.Helpers
  use PubSub.Helpers

  # Notification about a new timelane event:
  #   app: Application name
  #   module: Module within the app
  #   time: DateTime.t() for the event
  #   event: The name of the event
  #   metadata: map of metadata associated with the event
  define_channel("timelane_event", app: "", module: "", time: nil, event: "", metadata: %{})

end
