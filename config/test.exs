use Mix.Config

# config :logger, level: :debug
# config :logger, :console,
#   level: :debug,
#   format: "$date $time [$level] $metadata$message\n",
#   metadata: [:user_id]

# Logger configuration (override default here!)
config :logger,
  console: [level: :debug,
            format: Application.get_env(:opentimelane, :console_log_format)],
  handle_sasl_reports: false,
  handle_otp_reports: true

# Log file backends (override default here!)
config :logger,
  backends: [
    :console,
  ]
