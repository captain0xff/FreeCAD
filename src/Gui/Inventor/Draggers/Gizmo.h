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

#ifndef GUI_GIZMO_H
#define GUI_GIZMO_H

#include <vector>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/SbVec3f.h>
#include <QMetaObject>

#include <Base/Placement.h>

#include <FCGlobal.h>

class SoDragger;
class SoCamera;
class SoInteractionKit;

namespace Gui
{
class QuantitySpinBox;
class SoLinearDragger;
class SoLinearDraggerContainer;
class SoRotationDragger;
class SoRotationDraggerContainer;
class View3DInventorViewer;

struct GizmoPlacement
{
    SbVec3f pos;
    SbVec3f dir;
};

class GuiExport Gizmo
{
public:
    double multFactor = 1.0f;
    double addFactor = 0.0f;

    virtual ~Gizmo() = default;
    virtual SoInteractionKit* initDragger() = 0;
    virtual void uninitDragger() = 0;

    virtual GizmoPlacement getDraggerPlacement() = 0;
    virtual void setDraggerPlacement(const SbVec3f& pos, const SbVec3f& dir) = 0;
    void setDraggerPlacement(const Base::Vector3d& pos, const Base::Vector3d& dir);

    virtual void setGeometryScale(float scale) = 0;
    virtual void orientAlongCamera([[maybe_unused]] SoCamera* camera) {};
    bool delayedUpdateEnabled();

protected:
    QuantitySpinBox* property = nullptr;
    double initialValue;
};

class GuiExport LinearGizmo: public Gizmo
{
public:
    LinearGizmo(QuantitySpinBox* property);
    ~LinearGizmo() override = default;

    SoInteractionKit* initDragger() override;
    void uninitDragger() override;

    // Returns the position and rotation of the base of the dragger
    GizmoPlacement getDraggerPlacement() override;
    void setDraggerPlacement(const SbVec3f& pos, const SbVec3f& dir) override;
    void setDraggerPlacement(Base::Placement placement);
    void reverseDir();
    // Returns the drag distance from the base of the feature
    double getDragLength();
    void setDragLength(double dragLength);
    void setGeometryScale(float scale) override;
    SoLinearDraggerContainer* getDraggerContainer();
    void setProperty(QuantitySpinBox* property);

private:
    SoLinearDragger* dragger = nullptr;
    SoLinearDraggerContainer* draggerContainer = nullptr;
    QMetaObject::Connection connection;

    void draggingStarted();
    void draggingFinished();
    void draggingContinued();

    using inherited = Gizmo;
};

class GuiExport RotationGizmo: public Gizmo
{
public:
    RotationGizmo(QuantitySpinBox* property);
    ~RotationGizmo() override;

    SoInteractionKit* initDragger() override;
    void uninitDragger() override;

    // Distance between the linear gizmo base and rotation gizmo
    double sepDistance = 0;

    // Returns the position and rotation of the base of the dragger
    GizmoPlacement getDraggerPlacement() override;
    void setDraggerPlacement(const SbVec3f& pos, const SbVec3f& dir) override;
    // The two gizmos are separated by sepDistance units
    void placeOverLinearGizmo(LinearGizmo* gizmo);
    void placeBelowLinearGizmo(LinearGizmo* gizmo);
    // Returns the rotation angle wrt the normal axis
    double getRotAngle();
    void setRotAngle(double angle);
    void setGeometryScale(float scale) override;
    SoRotationDraggerContainer* getDraggerContainer();
    void orientAlongCamera(SoCamera* camera) override;
    void setProperty(QuantitySpinBox* property);

private:
    SoRotationDragger* dragger = nullptr;
    SoRotationDraggerContainer* draggerContainer = nullptr;
    SoFieldSensor translationSensor;
    LinearGizmo* linearGizmo = nullptr;
    bool automaticOrientation = false;
    QMetaObject::Connection connection;

    void draggingStarted();
    void draggingFinished();
    void draggingContinued();
    static void translationSensorCB(void* data, SoSensor* sensor);

    using inherited = Gizmo;
};

class GuiExport DirectedRotationGizmo: public RotationGizmo
{
public:
    DirectedRotationGizmo(QuantitySpinBox* property);

    SoInteractionKit* initDragger() override;

    void flipArrow();

private:
    using inherited = RotationGizmo;
};

class GuiExport RadialGizmo: public RotationGizmo
{
public:
    RadialGizmo(QuantitySpinBox* property);

    SoInteractionKit* initDragger() override;

    void setRadius(float radius);
    void flipArrow();

private:
    using inherited = RotationGizmo;
};

class GuiExport Gizmos: public SoBaseKit
{
    SO_KIT_HEADER(Gizmos);
    SO_KIT_CATALOG_ENTRY_HEADER(annotation);
    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(toggleSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(geometry);

public:
    static void initClass();
    Gizmos();
    ~Gizmos() override;

    SoSFBool visible;

    void initGizmos();
    void uninitGizmos();

    template<typename T = Gizmo>
    T* getGizmo(int index)
    {
        assert(index >= 0 && index < static_cast<int>(gizmos.size()) && "index out of range!");
        T* ptr  = dynamic_cast<T*>(gizmos[index]);
        assert(ptr);

        return ptr;
    }

    void addGizmo(Gizmo* gizmo);
    void attachViewer(Gui::View3DInventorViewer* viewer, Base::Placement &origin);
    void setUpAutoScale(SoCamera* cameraIn);
    void calculateScaleAndOrientation();

    // Checks if the gizmos are enabled in the preferences
    static bool isEnabled();

private:
    std::vector<Gizmo*> gizmos;
    SoFieldSensor cameraSensor;
    SoFieldSensor cameraPositionSensor;

    static void cameraChangeCallback(void* data, SoSensor*);
    static void cameraPositionChangeCallback(void* data, SoSensor*);
};

}

#endif /* GUI_GIZMO_H */
