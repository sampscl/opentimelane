defmodule LoggerUtils do
  @moduledoc """
  Macros supporting logging
  """
  require Logger

  defmacro __using__(_opts) do
    quote do
      require Logger
      require LoggerUtils
    end
  end

  defmacro debug(string, metadata \\ []) do
    quote do
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(string)
      Logger.debug(log_string, unquote(metadata))
    end
  end

  defmacro info(string, metadata \\ []) do
    quote do
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(string)
      Logger.info(log_string, unquote(metadata))
    end
  end

  defmacro warn(string, metadata \\ []) do
    quote do
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(string)
      Logger.warn(log_string, unquote(metadata))
    end
  end

  defmacro error(string, metadata \\ []) do
    quote do
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(string)
      Logger.error(log_string, unquote(metadata))
    end
  end

  defmacro log(level, string) do
    quote do
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(string)
      case unquote(level) do
        :debug -> Logger.debug(log_string)
        :info -> Logger.info(log_string)
        :warn -> Logger.warn(log_string)
        :error -> Logger.error(log_string)
      end
    end
  end

  defmacro inspect(item, level, lambda) do
    quote do
      real_item = unquote(item)
      {f, a} = __ENV__.function
      l = __ENV__.line
      file = __ENV__.file
      path = "./#{Path.relative_to_cwd(file)}" |> String.replace(~r/.+?\/apps\//, "", global: false)
      log_string = "#{node()} #{path}:#{l} in #{f}/#{a}: " <> unquote(lambda).(unquote(item))
      case unquote(level) do
        :debug -> Logger.debug(log_string)
        :info -> Logger.info(log_string)
        :warn -> Logger.warn(log_string)
        :error -> Logger.error(log_string)
      end
      real_item
    end
  end

end
