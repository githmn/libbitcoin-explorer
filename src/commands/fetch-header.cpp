/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <bitcoin/explorer/commands/fetch-header.hpp>

#include <iostream>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <bitcoin/explorer/callback_state.hpp>
#include <bitcoin/explorer/define.hpp>
#include <bitcoin/explorer/display.hpp>
#include <bitcoin/explorer/config/encoding.hpp>
#include <bitcoin/explorer/config/header.hpp>
#include <bitcoin/explorer/utility.hpp>

namespace libbitcoin {
namespace explorer {
namespace commands {
using namespace bc::client;
using namespace bc::explorer::config;

console_result fetch_header::invoke(std::ostream& output, std::ostream& error)
{
    // Bound parameters.
    const auto height = get_height_option();
    const hash_digest& hash = get_hash_option();
    const encoding& encoding = get_format_option();
    const auto connection = get_connection(*this);

    obelisk_client client(connection);

    if (!client.connect(connection))
    {
        display_connection_failure(error, connection.server);
        return console_result::failure;
    }

    callback_state state(error, output, encoding);

    auto on_done = [&state](const chain::header& header)
    {
        state.output(bc::property_tree(header));
    };

    auto on_error = [&state](const code& error)
    {
        state.succeeded(error);
    };

    // Height is ignored if both are specified.
    // Use the null_hash as sentinel to determine whether to use height or hash.
    if (hash == null_hash)
        client.blockchain_fetch_block_header(on_error, on_done, height);
    else
        client.blockchain_fetch_block_header(on_error, on_done, hash);

    client.wait();

    return state.get_result();
}


} //namespace commands
} //namespace explorer
} //namespace libbitcoin
