// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 Sayantan Deb <sayantandebin[at]gmail.com>           *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#include "PreCompiled.h"

#include "OnViewParameters.h"

#ifndef _PreComp_
#endif

#include <Gui/QuantitySpinBox.h>

using namespace Gui;

OnViewParameters::OnViewParameters(QuantitySpinBox* property): property(property)
{
	ovp = new QuantitySpinBox;
	ovp->setValue(property->value());
	ovp->setMinimum(property->minimum());
	ovp->setMaximum(property->maximum());
	ovp->setButtonSymbols(QAbstractSpinBox::NoButtons);
	ovp->setKeyboardTracking(false);
	ovp->hide();

	// Update the ovp when the property changes
	QuantitySpinBox::connect(
		property, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
		[this] (double value) {
			if (ovp->rawValue() != value) {
		    	ovp->setValue(value);
			}
		}
	);

	// Update the property when the ovp changes
	QuantitySpinBox::connect(
		ovp, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
		[property] (double value) {
			if (property->rawValue() != value) {
		    	property->setValue(value);
			}
		}
	);
}
