/*
    Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "config.h"

#ifdef SVG_SUPPORT
#include "SVGFEHelpersCg.h"

#include "Color.h"
#include "SVGDistantLightSource.h"
#include "SVGLightSource.h"
#include "SVGPointLightSource.h"
#include "SVGSpotLightSource.h"

#import "WKDistantLightFilter.h"
#import "WKNormalMapFilter.h"
#import "WKPointLightFilter.h"
#import "WKSpotLightFilter.h"

namespace WebCore {

CIVector* getVectorForChannel(SVGChannelSelectorType channel)
{
    switch (channel) {
    case SVG_CHANNEL_UNKNOWN:
        return nil;    
    case SVG_CHANNEL_R:
        return [CIVector vectorWithX:1.0 Y:0.0 Z:0.0 W:0.0];
    case SVG_CHANNEL_G:
        return [CIVector vectorWithX:0.0 Y:1.0 Z:0.0 W:0.0];
    case SVG_CHANNEL_B:
        return [CIVector vectorWithX:0.0 Y:0.0 Z:1.0 W:0.0];
    case SVG_CHANNEL_A:
        return [CIVector vectorWithX:0.0 Y:0.0 Z:0.0 W:1.0];
    default:
        return [CIVector vectorWithX:0.0 Y:0.0 Z:0.0 W:0.0];
    }
}

CIColor* ciColor(const Color& c)
{
    CGColorRef colorCG = cgColor(c);
    CIColor* colorCI = [CIColor colorWithCGColor:colorCG];
    CGColorRelease(colorCG);
    return colorCI;
}

// Lighting
CIFilter* getPointLightVectors(CIFilter* normals, CIVector* lightPosition, float surfaceScale)
{
    CIFilter* filter;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    filter = [CIFilter filterWithName:@"WKPointLight"];
    if (!filter)
        return nil;
    [filter setDefaults];
    [filter setValue:[normals valueForKey:@"outputImage"] forKey:@"inputNormalMap"];
    [filter setValue:lightPosition forKey:@"inputLightPosition"];
    [filter setValue:[NSNumber numberWithFloat:surfaceScale] forKey:@"inputSurfaceScale"];
    return filter;
    END_BLOCK_OBJC_EXCEPTIONS;
    return nil;
}

CIFilter* getLightVectors(CIFilter* normals, const SVGLightSource* light, float surfaceScale)
{
    [WKDistantLightFilter class];
    [WKPointLightFilter class];
    [WKSpotLightFilter class];

    CIFilter* filter = nil;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;

    switch (light->type()) {
    case LS_DISTANT:
    {
        const SVGDistantLightSource* dlight = static_cast<const SVGDistantLightSource*>(light);

        filter = [CIFilter filterWithName:@"WKDistantLight"];
        if (!filter)
            return nil;
        [filter setDefaults];

        float azimuth = dlight->azimuth();
        float elevation = dlight->elevation();
        azimuth = deg2rad(azimuth);
        elevation = deg2rad(elevation);
        float Lx = cos(azimuth)*cos(elevation);
        float Ly = sin(azimuth)*cos(elevation);
        float Lz = sin(elevation);

        [filter setValue:[normals valueForKey:@"outputImage"] forKey:@"inputNormalMap"];
        [filter setValue:[CIVector vectorWithX:Lx Y:Ly Z:Lz] forKey:@"inputLightDirection"];
        return filter;
    }
    case LS_POINT:
    {
        const SVGPointLightSource* plight = static_cast<const SVGPointLightSource*>(light);
        return getPointLightVectors(normals, [CIVector vectorWithX:plight->position().x() Y:plight->position().y() Z:plight->position().z()], surfaceScale);
    }
    case LS_SPOT:
    {
        const SVGSpotLightSource* slight = static_cast<const SVGSpotLightSource*>(light);
        filter = [CIFilter filterWithName:@"WKSpotLight"];
        if (!filter)
            return nil;

        CIFilter* pointLightFilter = getPointLightVectors(normals, [CIVector vectorWithX:slight->position().x() Y:slight->position().y() Z:slight->position().z()], surfaceScale);
        if (!pointLightFilter)
            return nil;
        [filter setDefaults];

        [filter setValue:[pointLightFilter valueForKey:@"outputImage"] forKey:@"inputLightVectors"];
        [filter setValue:[CIVector vectorWithX:slight->direction().x() Y:slight->direction().y() Z:slight->direction().z()] forKey:@"inputLightDirection"];
        [filter setValue:[NSNumber numberWithFloat:slight->specularExponent()] forKey:@"inputSpecularExponent"];
        [filter setValue:[NSNumber numberWithFloat:deg2rad(slight->limitingConeAngle())] forKey:@"inputLimitingConeAngle"];
        return filter;
    }
    }

    END_BLOCK_OBJC_EXCEPTIONS;
    return nil;
}

CIFilter* getNormalMap(CIImage* bumpMap, float scale)
{
    [WKNormalMapFilter class];
    CIFilter* filter;
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    filter = [CIFilter filterWithName:@"WKNormalMap"];
    [filter setDefaults];

    [filter setValue:bumpMap forKey:@"inputImage"];
    [filter setValue:[NSNumber numberWithFloat:scale] forKey:@"inputSurfaceScale"];
    return filter;
    END_BLOCK_OBJC_EXCEPTIONS;
    return nil;
}

}

#endif // SVG_SUPPORT
