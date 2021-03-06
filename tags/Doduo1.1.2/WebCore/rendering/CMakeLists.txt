LIST (APPEND RENDERING_SRC
    rendering/AutoTableLayout.cpp
    rendering/CounterNode.cpp
    rendering/EllipsisBox.cpp
    rendering/FixedTableLayout.cpp
    rendering/HitTestResult.cpp
    rendering/InlineBox.cpp
    rendering/InlineFlowBox.cpp
    rendering/InlineTextBox.cpp
    rendering/LayoutState.cpp
    rendering/ListMarkerBox.cpp
    rendering/MediaControlElements.cpp
    rendering/RenderApplet.cpp
    rendering/RenderArena.cpp
    rendering/RenderBR.cpp
    rendering/RenderBlock.cpp
    rendering/RenderBox.cpp
    rendering/RenderButton.cpp
    rendering/RenderContainer.cpp
    rendering/RenderCounter.cpp
    rendering/RenderFieldset.cpp
    rendering/RenderFileUploadControl.cpp
    rendering/RenderFlexibleBox.cpp
    rendering/RenderFlow.cpp
    rendering/RenderFrame.cpp
    rendering/RenderFrameSet.cpp
    rendering/RenderHTMLCanvas.cpp
    rendering/RenderImage.cpp
    rendering/RenderImageGeneratedContent.cpp
    rendering/RenderInline.cpp
    rendering/RenderLayer.cpp
    rendering/RenderLegend.cpp
    rendering/RenderListBox.cpp
    rendering/RenderListItem.cpp
    rendering/RenderListMarker.cpp
    rendering/RenderMarquee.cpp
    rendering/RenderMedia.cpp
    rendering/RenderMenuList.cpp
    rendering/RenderObject.cpp
    rendering/RenderPart.cpp
    rendering/RenderPartObject.cpp
    rendering/RenderReplaced.cpp
    rendering/RenderReplica.cpp
    rendering/RenderScrollbar.cpp
    rendering/RenderScrollbarPart.cpp
    rendering/RenderScrollbarTheme.cpp
    rendering/RenderSlider.cpp
    rendering/RenderTable.cpp
    rendering/RenderTableCell.cpp
    rendering/RenderTableCol.cpp
    rendering/RenderTableRow.cpp
    rendering/RenderTableSection.cpp
    rendering/RenderText.cpp
    rendering/RenderTextControl.cpp
    rendering/RenderTextFragment.cpp
    rendering/RenderTheme.cpp
#    rendering/RenderThemeSafari.cpp
#    rendering/RenderThemeWin.cpp
    rendering/RenderTreeAsText.cpp
    rendering/RenderVideo.cpp
    rendering/RenderView.cpp
    rendering/RenderWidget.cpp
    rendering/RenderWordBreak.cpp
    rendering/RootInlineBox.cpp
    rendering/TextControlInnerElements.cpp
    rendering/bidi.cpp
    rendering/break_lines.cpp

	rendering/style/Animation.cpp
	rendering/style/AnimationList.cpp
	rendering/style/BindingURI.cpp
	rendering/style/ContentData.cpp
	rendering/style/CounterDirectives.cpp
	rendering/style/FillLayer.cpp
	rendering/style/KeyframeList.cpp
	rendering/style/MatrixTransformOperation.cpp
	rendering/style/NinePieceImage.cpp
	rendering/style/RenderStyle.cpp
	rendering/style/RotateTransformOperation.cpp
	rendering/style/ScaleTransformOperation.cpp
	rendering/style/ShadowData.cpp
	rendering/style/SkewTransformOperation.cpp
	rendering/style/StyleBackgroundData.cpp
	rendering/style/StyleBoxData.cpp
	rendering/style/StyleCachedImage.cpp
	rendering/style/StyleFlexibleBoxData.cpp
	rendering/style/StyleGeneratedImage.cpp
	rendering/style/StyleMarqueeData.cpp
	rendering/style/StyleMultiColData.cpp
	rendering/style/StyleSurroundData.cpp
	rendering/style/StyleTransformData.cpp
	rendering/style/StyleVisualData.cpp
	rendering/style/TransformOperations.cpp
	rendering/style/TranslateTransformOperation.cpp
	rendering/style/StyleInheritedData.cpp
	rendering/style/StyleRareInheritedData.cpp
	rendering/style/StyleRareNonInheritedData.cpp

)
IF (WEBKIT_USE_SVG)
    LIST (APPEND WEBCORE_SRC
	    rendering/PointerEventsHitRules.cpp
	    rendering/RenderForeignObject.cpp
	    rendering/RenderPath.cpp
	    rendering/RenderSVGBlock.cpp
	    rendering/RenderSVGContainer.cpp
	    rendering/RenderSVGGradientStop.cpp
	    rendering/RenderSVGHiddenContainer.cpp
	    rendering/RenderSVGImage.cpp
	    rendering/RenderSVGInline.cpp
	    rendering/RenderSVGInlineText.cpp
	    rendering/RenderSVGRoot.cpp
	    rendering/RenderSVGText.cpp
	    rendering/RenderSVGTextPath.cpp
	    rendering/RenderSVGTransformableContainer.cpp
	    rendering/RenderSVGTSpan.cpp
	    rendering/RenderSVGViewportContainer.cpp
	    rendering/SVGCharacterLayoutInfo.cpp
	    rendering/SVGInlineFlowBox.cpp
	    rendering/SVGInlineTextBox.cpp
	    rendering/SVGRenderSupport.cpp
	    rendering/SVGRenderTreeAsText.cpp
	    rendering/SVGRootInlineBox.cpp
	    rendering/style/SVGRenderStyle.cpp
	    rendering/style/SVGRenderStyleDefs.cpp
    )
ENDIF (WEBKIT_USE_SVG)
