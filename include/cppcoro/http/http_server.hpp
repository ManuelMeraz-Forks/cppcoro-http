/**
 * @file cppcoro/http_server.hpp
 */
#pragma once

#include <cppcoro/task.hpp>
#include <cppcoro/tcp/tcp.hpp>
#include <cppcoro/http/http_connection.hpp>
#include <cppcoro/operation_cancelled.hpp>

#include <utility>
#include <vector>
#include <iostream>

namespace cppcoro::http {

    class server : protected tcp::server
    {
    public:
        using tcp::server::server;
        using tcp::server::stop;
        using tcp::server::service;
        using connection_type = connection<server>;

        task<connection_type> listen() {
            auto conn_generator = accept();
            while (!cs_.is_cancellation_requested()) {
                spdlog::debug("listening for new connection on {}", socket_.local_endpoint());
                connection_type conn{*this, std::move(co_await accept())};
                spdlog::debug("{} incoming connection: {}", conn.to_string(), conn.peer_address());
                co_return conn;
            }
            throw operation_cancelled{};
        }

    private:
        cppcoro::cancellation_source cancellation_source_;
    };
} // namespace cpporo::http
