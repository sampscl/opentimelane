defmodule PubSub do
  @moduledoc """
  PubSub module contains modules and functions related to pub/sub. Duh.
  """
  import PubSub.Helpers
  use PubSub.Helpers

  # Notification channel about motion events. Model is a %Db.Models.MotionEvent{}
  # and event must be in [:begin, :end, :delete]
  # define_channel("motion_event_notice", model: nil, event: :begin)

end
