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

#ifndef PAGEPLAY_H
#define PAGEPLAY_H

#include <QWidget>

class QPushButton;
class AbstractChannel;
class PlayFork;
class MyStatusBar;

class PagePlay : public QWidget
{
       Q_OBJECT
    public:
        PagePlay( QWidget* parent = 0 );
        ~PagePlay();
        QPushButton* buttonclose;
    private:
        QPushButton* buttonlaunch;
        QPushButton* buttonplayer;

        AbstractChannel* channel;
        PlayFork* playfork;
        MyStatusBar* statusbar;

        void onAppendToHistory();

    private slots:
        void onButtonPlayerToggled( bool );
        void onButtonLaunchClicked( bool );
};

#endif // PAGEPLAY_H
