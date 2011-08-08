//
//
//  @ Project : ircppbot
//  @ File Name : Bot.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "include/Session.h"
#include "include/RecvData.h"
#include "include/SendData.h"
#include <cstdlib>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


Session::Session(boost::asio::io_service& io_service) : non_ssl_socket_(io_service)
{
}


Session::~Session()
{
    SendData::Instance().DelSession(this);
    std::cout << "close session" << std::endl;
}


boost::asio::ip::tcp::socket& Session::non_ssl_socket()
{
    return non_ssl_socket_;
}

void Session::start()
{
    SendData::Instance().AddSession(this);
    read();
    std::cout << "start read thread" << std::endl;
}

void Session::read()
{
    //start to receive
    memset( data_, '\0', max_length );
    non_ssl_socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&Session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Session::Write(std::string data)
{
    char write_data_[max_length];
    memset( write_data_, '\0', max_length );
    strcpy(write_data_, data.c_str());
    size_t request_length = std::strlen(write_data_);
    boost::asio::write(non_ssl_socket_, boost::asio::buffer(write_data_, request_length));
}


void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    try
    {
        if (error == boost::asio::error::eof)
        {
            std::cout << "closed by client" << std::endl;
            delete this;
            return;
        }
        else if (error)
        {
            throw boost::system::system_error(error); // Some other error.
            delete this;
            return;
        }
        else
        {
            std::string buf_data = std::string(data_);
            boost::trim(buf_data);
            std::cout << "void Session::read() " << buf_data << std::endl;
            RecvData::Instance().AddRecvQueue(buf_data);
            memset( data_, '\0', max_length );
            non_ssl_socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                            boost::bind(&Session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}
