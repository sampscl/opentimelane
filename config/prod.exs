use Mix.Config

# config :logger, level: :info
# config :logger, :console,
#   level: :info,
#   format: "$date $time [$level] $metadata$message\n",
#   metadata: [:user_id]

# config sasl
config :sasl,
  sasl_error_logger: {:file, '#{Application.get_env(:opentimelane, :base_log_dir)}/sasl.log'},
  errlog_type: :all

# Logger configuration
config :logger,
  console: [level: :info,
            format: Application.get_env(:opentimelane, :console_log_format)],
  handle_sasl_reports: false,
  handle_otp_reports: true

# Log file backends
config :logger,
  backends: [
    :console,
    {LoggerFileBackend, :debug_log},
    {LoggerFileBackend, :console_log},
    {LoggerFileBackend, :error_log},
  ]
config :logger, :console_log,
  path: "#{Application.get_env(:opentimelane, :base_log_dir)}/console.log",
  format: Application.get_env(:opentimelane, :disk_log_format),
  level: :info
config :logger, :error_log,
  path: "#{Application.get_env(:opentimelane, :base_log_dir)}/error.log",
  format: Application.get_env(:opentimelane, :disk_log_format),
  level: :error
config :logger, :debug_log,
  path: "#{Application.get_env(:opentimelane, :base_log_dir)}/debug.log",
  format: Application.get_env(:opentimelane, :disk_log_format),
  level: :debug
