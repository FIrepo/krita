/*
 * This file is part of Krita
 *
 * Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_WDG_FASTCOLORTRANSFER_H
#define KIS_WDG_FASTCOLORTRANSFER_H

#include <kis_config_widget.h>

class Ui_WdgFastColorTransfer;

/**
 @author Cyrille Berger <cberger@cberger.net>
*/
class KisWdgFastColorTransfer : public KisConfigWidget
{
public:
    KisWdgFastColorTransfer(QWidget * parent);
    ~KisWdgFastColorTransfer() override;
    void setConfiguration(const KisPropertiesConfigurationSP) override;
    inline const Ui_WdgFastColorTransfer* widget() const {
        return m_widget;
    }
    KisPropertiesConfigurationSP configuration() const override;
private:
    Ui_WdgFastColorTransfer* m_widget;
};

#endif
