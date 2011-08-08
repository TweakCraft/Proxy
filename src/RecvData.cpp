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

#include "include/RecvData.h"
RecvData::RecvData()
{
}

RecvData::~RecvData()
{
    stop();
}

void RecvData::stop()
{
    recv = false;
    while (!RecvQueue.empty())
    {
        RecvQueue.pop();
    }
    RecvAvailable.notify_all();
}

void RecvData::init()
{
    while (!RecvQueue.empty())
    {
        RecvQueue.pop();
    }
    recv = true;
}

void RecvData::AddRecvQueue(std::string data)
{
    boost::mutex::scoped_lock lock(RecvMutex);
    RecvQueue.push(data);
    RecvAvailable.notify_one();
}

std::string RecvData::GetRecvQueue()
{
    boost::mutex::scoped_lock lock(RecvMutex);
    std::string temp = "";
    while(RecvQueue.empty())
    {
        RecvAvailable.wait(lock);
    }
    if (recv == true)
    {
        temp = RecvQueue.front();
        RecvQueue.pop();
    }
    return temp;
}


