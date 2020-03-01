# This file is responsible for configuring your application
# and its dependencies with the aid of the Mix.Config module.
use Mix.Config

# Common configs
Application.put_env(:opentimelane, :base_log_dir, "/var/log/opentimelane")
Application.put_env(:opentimelane, :console_log_format, "$time [$level] $levelpad$message\n")
Application.put_env(:opentimelane, :disk_log_format, "$date $time [$level] $levelpad$message\n")

import_config "#{Mix.env}.exs"

# Sample configuration (overrides the imported configuration above):
#
#     config :logger, :console,
#       level: :info,
#       format: "$date $time [$level] $metadata$message\n",
#       metadata: [:user_id]
