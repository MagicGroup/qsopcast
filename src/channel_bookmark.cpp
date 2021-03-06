/*
    This file is part of qsopcast project.
    Copyright (C) 2005, 2006  Liu Di <liudidi@gmail.com>
    Copyright (C) 2007  Wei Lian <lianwei3@gmail.com>
    Copyright (C) 2009, 2010  Ni Hui <shuizhuyuanluo@126.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QObject>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QSettings>

#include "channel_pplive.h"
#include "channel_ppstream.h"
#include "channel_sopcast.h"

#include "channel_bookmark.h"

ChannelListBookmark* ChannelListBookmark::m_self = 0;

ChannelListBookmark* ChannelListBookmark::self()
{
    if ( !m_self )
        m_self = new ChannelListBookmark;
    return m_self;
}

ChannelListBookmark::ChannelListBookmark( QObject* parent ) :
    AbstractChannelList( parent )
{
}

ChannelListBookmark::~ChannelListBookmark()
{
    saveChannels();
    m_self = 0;
}

void ChannelListBookmark::retrieveChannels()
{
    QDomDocument doc;
    QSettings settings;
    QString filepath = settings.value( "/qsopcast/bookmark_fileurl", QDir::homePath() + "/.qsopcast/bookmark.xml" ).toString();
    QFile file( filepath );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    if ( !doc.setContent( &file ) ) {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    QDomNode bookmarknode = docElem.firstChild();
    while ( !bookmarknode.isNull() ) {
        if ( bookmarknode.isElement() && bookmarknode.nodeName() == "channel" ) {
            QString channelname;
            QString channelurl;
            QDomNode channelnode = bookmarknode.firstChild();
            while ( !channelnode.isNull() ) {
                if ( channelnode.nodeName() == "name" )
                    channelname = channelnode.firstChild().nodeValue();
                else if ( channelnode.nodeName() == "url" )
                    channelurl = channelnode.firstChild().nodeValue();
                channelnode = channelnode.nextSibling();
            }
            /// guess the channel source type
            AbstractChannel* channel;
            if ( channelurl.startsWith( "synacast://" ) )
                channel = new ChannelPPLive( this );
            else if ( channelurl.startsWith( "pps://" ) )
                channel = new ChannelPPStream( this );
            else if ( channelurl.startsWith( "sop://" ) )
                channel = new ChannelSopcast( this );
            else
                continue;// NOTE: can not decide it, throw this channel  ;)
            /// guess the channel media format
            if ( channelurl.endsWith( ".wmv" ) || channelurl.endsWith( ".asf" ) )
                channel->m_format = wmv;
            else if ( channelurl.endsWith( ".rmvb" ) || channelurl.endsWith( ".rm" ) )
                channel->m_format = rmvb;
            else if ( channelurl.endsWith( ".mp3" ) )
                channel->m_format = mp3;
            else
                channel->m_format = wmv;// NOTE: hmm... if wmv is good here?
            channel->m_name = channelname;
            channel->m_url = channelurl;
            channel->m_kbps = 0;/// do not record it
            channel->m_sourcetype = (source_t)( channel->m_sourcetype | BOOKMARK );
            channels.append( channel );
        }
        bookmarknode = bookmarknode.nextSibling();
    }

    if ( channels.count() > 0 ) {
        QHash< QString, QList<AbstractChannel*> > hashchannels;
        hashchannels[ tr( "My Bookmarks" ) ] = channels;
        emit channellistRetrieved( hashchannels );
    }
}

void ChannelListBookmark::saveChannels()
{
    /// build dom tree
    QDomDocument doc;
    QDomElement bookmark = doc.createElement( "bookmark" );
    doc.appendChild( bookmark );

    QList<AbstractChannel*>::const_iterator it_channel = channels.constBegin();
    while ( it_channel != channels.constEnd() ) {
        /// append channel node
        QDomElement channel = doc.createElement( "channel" );
        bookmark.appendChild( channel );
        QDomElement name = doc.createElement( "name" );
        channel.appendChild( name );
        QDomElement url = doc.createElement( "url" );
        channel.appendChild( url );
        QDomText namestring = doc.createTextNode( (*it_channel)->m_name );
        name.appendChild( namestring );
        QDomText urlstring = doc.createTextNode( (*it_channel)->m_url );
        url.appendChild( urlstring );
        ++it_channel;
    }

    QString xml = doc.toString();

    /// write to file
    QSettings settings;
    QString filepath = settings.value( "/qsopcast/bookmark_fileurl", QDir::homePath() + "/.qsopcast/bookmark.xml" ).toString();
    QFile file( filepath );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return;
    QTextStream out( &file );
    out.setCodec( "UTF-8" );
    out << xml;
    file.close();
}

void ChannelListBookmark::addChannel( AbstractChannel* channel )
{
    channels.append( channel );
}

void ChannelListBookmark::removeChannel( const QString& name )
{
    int i = 0;
    for ( ; i<channels.size(); ++i ) {
        if ( channels[i]->m_name == name )
            break;
    }
    delete channels[i];
    channels.removeAt( i );
}

#include "channel_bookmark.moc"
