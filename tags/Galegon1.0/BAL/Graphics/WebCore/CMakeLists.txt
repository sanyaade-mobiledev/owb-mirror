##################################################
# Graphics support part.                         #
##################################################

set(GRAPHICS_DIR ${BAL_DIR}/Graphics/WebCore)

create_include_link(${GRAPHICS_DIR}/WK/BCAnimationWK.h BAL/Animation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCAnimationListWK.h BAL/AnimationList.h)
create_include_link(${GRAPHICS_DIR}/WK/BCBitmapImageWK.h BAL/BitmapImage.h)
create_include_link(${GRAPHICS_DIR}/WK/BCColorWK.h BAL/Color.h)
create_include_link(${GRAPHICS_DIR}/WK/BCDashArrayWK.h BAL/DashArray.h)
create_include_link(${GRAPHICS_DIR}/WK/BCDragDataWK.h BAL/DragData.h)
create_include_link(${GRAPHICS_DIR}/WK/BCDragImageWK.h BAL/DragImage.h)
create_include_link(${GRAPHICS_DIR}/WK/BCFloatPoint3DWK.h BAL/FloatPoint3D.h)
create_include_link(${GRAPHICS_DIR}/WK/BCFloatPointWK.h BAL/FloatPoint.h)
create_include_link(${GRAPHICS_DIR}/WK/BCFloatQuadWK.h BAL/FloatQuad.h)
create_include_link(${GRAPHICS_DIR}/WK/BCFloatRectWK.h BAL/FloatRect.h)
create_include_link(${GRAPHICS_DIR}/WK/BCFloatSizeWK.h BAL/FloatSize.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGeneratedImageWK.h BAL/GeneratedImage.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGeneratorWK.h BAL/Generator.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGradientWK.h BAL/Gradient.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGraphicsContextPrivateWK.h BAL/GraphicsContextPrivate.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGraphicsContextWK.h BAL/GraphicsContext.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGraphicsLayerClientWK.h BAL/GraphicsLayerClient.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGraphicsLayerWK.h BAL/GraphicsLayer.h)
create_include_link(${GRAPHICS_DIR}/WK/BCGraphicsTypesWK.h BAL/GraphicsTypes.h)
create_include_link(${GRAPHICS_DIR}/WK/BCIdentifyTransformOperationWK.h BAL/IdentityTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCImageWK.h BAL/Image.h)
create_include_link(${GRAPHICS_DIR}/WK/BCImageBufferWK.h BAL/ImageBuffer.h)
create_include_link(${GRAPHICS_DIR}/WK/BCImageObserverWK.h BAL/ImageObserver.h)
create_include_link(${GRAPHICS_DIR}/WK/BCImageSourceWK.h BAL/ImageSource.h)
create_include_link(${GRAPHICS_DIR}/WK/BCIntPointWK.h BAL/IntPoint.h)
create_include_link(${GRAPHICS_DIR}/WK/BCIntRectWK.h BAL/IntRect.h)
create_include_link(${GRAPHICS_DIR}/WK/BCIntSizeHashWK.h BAL/IntSizeHash.h)
create_include_link(${GRAPHICS_DIR}/WK/BCIntSizeWK.h BAL/IntSize.h)
create_include_link(${GRAPHICS_DIR}/WK/BCMatrixTransformOperationWK.h BAL/MatrixTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCPathTraversalStateWK.h BAL/PathTraversalState.h)
create_include_link(${GRAPHICS_DIR}/WK/BCPathWK.h BAL/Path.h)
create_include_link(${GRAPHICS_DIR}/WK/BCPatternWK.h BAL/Pattern.h)
create_include_link(${GRAPHICS_DIR}/WK/BCPenWK.h BAL/Pen.h)
create_include_link(${GRAPHICS_DIR}/WK/BCRotateTransformOperationWK.h BAL/RotateTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCScaleTransformOperationWK.h BAL/ScaleTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCSkewTransformOperationWK.h BAL/SkewTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCStrokeStyleApplierWK.h BAL/StrokeStyleApplier.h)
create_include_link(${GRAPHICS_DIR}/WK/BCTimingFunctionWK.h BAL/TimingFunction.h)
create_include_link(${GRAPHICS_DIR}/WK/BCTransformOperationWK.h BAL/TransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCTransformOperationsWK.h BAL/TransformOperations.h)
create_include_link(${GRAPHICS_DIR}/WK/BCTranslateTransformOperationWK.h BAL/TranslateTransformOperation.h)
create_include_link(${GRAPHICS_DIR}/WK/BCUnitBezierWK.h BAL/UnitBezier.h)

if(USE_GRAPHICS_GTK)
    create_include_link(${GRAPHICS_DIR}/Cairo/BCCairoPathCairo.h BAL/CairoPath.h)
    create_include_link(${GRAPHICS_DIR}/Cairo/BCGraphicsContextPlatformPrivateCairo.h BAL/GraphicsContextPlatformPrivateCairo.h)
    create_include_link(${GRAPHICS_DIR}/Cairo/BCImageBufferDataCairo.h BAL/ImageBufferData.h)
    create_include_link(${GRAPHICS_DIR}/WK/BCTransformationMatrixWK.h BAL/TransformationMatrix.h)
    
    aux_source_directory(${GRAPHICS_DIR}/Gtk WEBCORE_SRC)
    aux_source_directory(${GRAPHICS_DIR}/Cairo WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_QT)
    create_include_link(${GRAPHICS_DIR}/Qt/BCImageBufferDataQt.h BAL/ImageBufferData.h)
    create_include_link(${GRAPHICS_DIR}/Qt/BCImageDecoderQt.h BAL/ImageDecoderQt.h)
    create_include_link(${GRAPHICS_DIR}/Qt/BCStillImageQt.h BAL/StillImageQt.h)
    create_include_link(${GRAPHICS_DIR}/WK/BCTransformationMatrixWK.h BAL/TransformationMatrix.h)

    aux_source_directory(${GRAPHICS_DIR}/Qt WEBCORE_SRC)
endif(USE_GRAPHICS_QT)

if(USE_GRAPHICS_SDL)
    create_include_link(${GRAPHICS_DIR}/SDL/BCTransformationMatrixSDL.h BAL/TransformationMatrix.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCApplyTransparencySDL.h BAL/ApplyTransparencySDL.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCBitmapImageSDL.h BAL/BitmapImage.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCGradientSDL.h BAL/Gradient.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCGraphicsContextSDL.h BAL/GraphicsContext.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCIntPointSDL.h BAL/IntPoint.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCIntRectSDL.h BAL/IntRect.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCPathSDL.h BAL/Path.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCPatternSDL.h BAL/Pattern.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCGraphicsContextPlatformPrivateSDL.h BAL/GraphicsContextPlatformPrivateSDL.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCImageBufferSDL.h BAL/ImageBuffer.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCImageSourceSDL.h BAL/ImageSource.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCDragImageSDL.h BAL/DragImage.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCIntPointSDL.h BAL/IntPoint.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCColorSDL.h BAL/Color.h)
    create_include_link(${GRAPHICS_DIR}/SDL/BCGraphicsContextPrivateSDL.h BAL/GraphicsContextPrivate.h)

    aux_source_directory(${GRAPHICS_DIR}/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)

aux_source_directory(${GRAPHICS_DIR}/WK WEBCORE_SRC)
