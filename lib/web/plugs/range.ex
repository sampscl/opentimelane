defmodule Web.Plugs.Range do
  @moduledoc """
  This plug will serve files requested by range in the headers. If no range
  is requested, then the connection is simply returned to the pipeline.

  This is not compliant with RFC 7233.

  Thanks StackOverflow: https://stackoverflow.com/questions/36576801/serving-http-range-request-with-phoenix
  """
  @behaviour Plug
  #@allowed_methods ~w(GET HEAD)
  import Plug.Conn

  def init(opts), do: opts

  def call(conn, _opts) do
    if Enum.empty?(Plug.Conn.get_req_header(conn, "range")) do
      conn
    else
      base_dir = Surveillance.recording_dir()
      file_path = Path.join(base_dir, Path.basename(conn.request_path))
      if File.exists? file_path do

        stats = File.stat! file_path
        filesize = stats.size
        range =
          conn
          |> get_req_header("range")
          |> List.first

        req = ~r/bytes=([0-9]+)-([0-9]+)?/ |> Regex.run(range)

        {req_start, _} = req |> Enum.at(1) |> Integer.parse
        {req_end, _} = req |> Enum.at(2, filesize |> to_string) |> Integer.parse

        #file_end = (filesize - 2) |> to_string

        length = req_end - req_start + 1

        conn
        |> put_resp_header("Content-Type", "video/mp4")
        |> put_resp_header("Accept-Ranges", "bytes")
        |> put_resp_header("Content-Range", "bytes #{req_start}-#{req_end}/#{filesize}")
        |> send_file(206, file_path, req_start, length)
        |> halt
      else
        conn
      end
    end
  end
end
