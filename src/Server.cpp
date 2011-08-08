#include "include/Server.h"
#include <cstdlib>
#include <iostream>


Server::Server(boost::asio::io_service& io_service, unsigned short port)
    : io_service_(io_service), acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port))
{
    Session* new_session = new Session(io_service_);
    acceptor_.async_accept(new_session->non_ssl_socket(),
                            boost::bind(&Server::handle_accept, this, new_session, boost::asio::placeholders::error));
}

void Server::handle_accept(Session* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        new_session = new Session(io_service_);
        acceptor_.async_accept(new_session->non_ssl_socket(),
                                boost::bind(&Server::handle_accept, this, new_session, boost::asio::placeholders::error));
    }
    else
    {
        delete new_session;
    }
}

