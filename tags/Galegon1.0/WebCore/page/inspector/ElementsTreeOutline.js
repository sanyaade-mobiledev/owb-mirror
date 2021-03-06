
WebInspector.ElementsTreeOutline = function() {
    this.element = document.createElement("ol");
    this.element.addEventListener("mousedown", this._onmousedown.bind(this), false);
    this.element.addEventListener("dblclick", this._ondblclick.bind(this), false);
    this.element.addEventListener("mousemove", this._onmousemove.bind(this), false);
    this.element.addEventListener("mouseout", this._onmouseout.bind(this), false);

    TreeOutline.call(this, this.element);
    
    this.includeRootDOMNode = true;
    this.selectEnabled = false;
    this.rootDOMNode = null;
    this.focusedDOMNode = null;
}

WebInspector.ElementsTreeOutline.prototype = {
    get rootDOMNode()
    {
        return this._rootDOMNode;
    },

    set rootDOMNode(x)
    {
        if (objectsAreSame(this._rootDOMNode, x))
            return;

        this._rootDOMNode = x;

        this.update();
    },

    get focusedDOMNode()
    {
        return this._focusedDOMNode;
    },

    set focusedDOMNode(x)
    {
        if (objectsAreSame(this._focusedDOMNode, x)) {
            var nodeItem = this.revealNode(x);
            if (nodeItem)
                nodeItem.select();
            return;
        }

        this._focusedDOMNode = x;

        this.focusedNodeChanged();

        var nodeItem = this.revealNode(x);
        if (nodeItem)
            nodeItem.select();
    },
    
    update: function()
    {
        this.removeChildren();

        if (!this.rootDOMNode)
            return;

        var treeElement;
        if (this.includeRootDOMNode) {
            treeElement = new WebInspector.ElementsTreeElement(this.rootDOMNode);
            treeElement.selectable = this.selectEnabled;
            this.treeOutline.appendChild(treeElement);
        } else {
            // FIXME: this could use findTreeElement to reuse a tree element if it already exists
            var node = (Preferences.ignoreWhitespace ? firstChildSkippingWhitespace.call(this.rootDOMNode) : this.rootDOMNode.firstChild);
            while (node) {
                treeElement = new WebInspector.ElementsTreeElement(node);
                treeElement.selectable = this.selectEnabled;
                this.treeOutline.appendChild(treeElement);
                node = Preferences.ignoreWhitespace ? nextSiblingSkippingWhitespace.call(node) : node.nextSibling;
            }
        }

        this.updateSelection();
    },

    updateSelection: function()
    {
        if (!this.selectedTreeElement)
            return;
        var element = this.selectedTreeElement;
        element.updateSelection();
    },

    focusedNodeChanged: function(forceUpdate) {},

    revealNode: function(node)
    {
        var nodeItem = this.treeOutline.findTreeElement(node, isAncestorIncludingParentFrames.bind(this), parentNodeOrFrameElement.bind(this), objectsAreSame);
        if (!nodeItem)
            return;

        nodeItem.reveal();
        return nodeItem;
    },

    _treeElementFromEvent: function(event)
    {
        var outline = this.treeOutline;

        var root = this.element;

        // We choose this X coordinate based on the knowledge that our list
        // items extend nearly to the right edge of the outer <ol>.
        var x = root.totalOffsetLeft + root.offsetWidth - 20;

        var y = event.pageY;

        // Our list items have 1-pixel cracks between them vertically. We avoid
        // the cracks by checking slightly above and slightly below the mouse
        // and seeing if we hit the same element each time.
        var elementUnderMouse = outline.treeElementFromPoint(x, y);
        var elementAboveMouse = outline.treeElementFromPoint(x, y - 2);
        var element;
        if (elementUnderMouse === elementAboveMouse)
            element = elementUnderMouse;
        else
            element = outline.treeElementFromPoint(x, y + 2);

        return element;
    },

    _ondblclick: function(event)
    {
        var element = this._treeElementFromEvent(event);

        if (!element || !element.ondblclick)
            return;

        element.ondblclick(element, event);
    },

    _onmousedown: function(event)
    {
        var element = this._treeElementFromEvent(event);

        if (!element || element.isEventWithinDisclosureTriangle(event))
            return;

        element.select();
    },

    _onmousemove: function(event)
    {
        var element = this._treeElementFromEvent(event);
        if (!element)
            return;

        WebInspector.hoveredDOMNode = element.representedObject;
        WebInspector.forceHoverHighlight = element.selected;
    },

    _onmouseout: function(event)
    {
        if (event.target !== this.element)
            return;

        WebInspector.hoveredDOMNode = null;
        WebInspector.forceHoverHighlight = false;
    }
}

WebInspector.ElementsTreeOutline.prototype.__proto__ = TreeOutline.prototype;



WebInspector.ElementsTreeElement = function(node)
{
    var hasChildren = node.contentDocument || (Preferences.ignoreWhitespace ? (firstChildSkippingWhitespace.call(node) ? true : false) : node.hasChildNodes());
    var titleInfo = nodeTitleInfo.call(node, hasChildren, WebInspector.linkifyURL);

    if (titleInfo.hasChildren) 
        this.whitespaceIgnored = Preferences.ignoreWhitespace;

    TreeElement.call(this, titleInfo.title, node, titleInfo.hasChildren);
}

WebInspector.ElementsTreeElement.prototype = {
    updateSelection: function()
    {
        var listItemElement = this.listItemElement;
        if (!listItemElement)
            return;

        if (document.body.offsetWidth <= 0) {
            // The stylesheet hasn't loaded yet or the window is closed,
            // so we can't calculate what is need. Return early.
            return;
        }

        if (!this.selectionElement) {
            this.selectionElement = document.createElement("div");
            this.selectionElement.className = "selection selected";
            listItemElement.insertBefore(this.selectionElement, listItemElement.firstChild);
        }

        this.selectionElement.style.height = listItemElement.offsetHeight + "px";
    },

    onattach: function()
    {
        this.listItemElement.addEventListener("mousedown", this.onmousedown.bind(this), false);

        this._preventFollowingLinksOnDoubleClick();
    },

    _preventFollowingLinksOnDoubleClick: function()
    {
        var links = this.listItemElement.querySelectorAll("li > .webkit-html-tag > .webkit-html-attribute > .webkit-html-external-link, li > .webkit-html-tag > .webkit-html-attribute > .webkit-html-resource-link");
        if (!links)
            return;

        for (var i = 0; i < links.length; ++i)
            links[i].preventFollowOnDoubleClick = true;
    },

    onpopulate: function()
    {
        if (this.children.length || this.whitespaceIgnored !== Preferences.ignoreWhitespace)
            return;

        this.whitespaceIgnored = Preferences.ignoreWhitespace;

        this.updateChildren();
    },

    updateChildren: function(fullRefresh)
    {
        if (fullRefresh) {
            var selectedTreeElement = this.treeOutline.selectedTreeElement;
            if (selectedTreeElement && selectedTreeElement.hasAncestor(this))
                this.select();
            this.removeChildren();
        }

        var treeElement = this;
        var treeChildIndex = 0;

        function updateChildrenOfNode(node)
        {
            var treeOutline = treeElement.treeOutline;
            var child = (Preferences.ignoreWhitespace ? firstChildSkippingWhitespace.call(node) : node.firstChild);
            while (child) {
                var currentTreeElement = treeElement.children[treeChildIndex];
                if (!currentTreeElement || !objectsAreSame(currentTreeElement.representedObject, child)) {
                    // Find any existing element that is later in the children list.
                    var existingTreeElement = null;
                    for (var i = (treeChildIndex + 1); i < treeElement.children.length; ++i) {
                        if (objectsAreSame(treeElement.children[i].representedObject, child)) {
                            existingTreeElement = treeElement.children[i];
                            break;
                        }
                    }

                    if (existingTreeElement && existingTreeElement.parent === treeElement) {
                        // If an existing element was found and it has the same parent, just move it.
                        var wasSelected = existingTreeElement.selected;
                        treeElement.removeChild(existingTreeElement);
                        treeElement.insertChild(existingTreeElement, treeChildIndex);
                        if (wasSelected)
                            existingTreeElement.select();
                    } else {
                        // No existing element found, insert a new element.
                        var newElement = new WebInspector.ElementsTreeElement(child);
                        newElement.selectable = treeOutline.selectEnabled;
                        treeElement.insertChild(newElement, treeChildIndex);
                    }
                }

                child = Preferences.ignoreWhitespace ? nextSiblingSkippingWhitespace.call(child) : child.nextSibling;
                ++treeChildIndex;
            }
        }

        // Remove any tree elements that no longer have this node (or this node's contentDocument) as their parent.
        for (var i = (this.children.length - 1); i >= 0; --i) {
            if ("elementCloseTag" in this.children[i])
                continue;

            var currentChild = this.children[i];
            var currentNode = currentChild.representedObject;
            var currentParentNode = currentNode.parentNode;

            if (objectsAreSame(currentParentNode, this.representedObject))
                continue;
            if (this.representedObject.contentDocument && objectsAreSame(currentParentNode, this.representedObject.contentDocument))
                continue;

            var selectedTreeElement = this.treeOutline.selectedTreeElement;
            if (selectedTreeElement && (selectedTreeElement === currentChild || selectedTreeElement.hasAncestor(currentChild)))
                this.select();

            this.removeChildAtIndex(i);

            if (this.treeOutline.panel && currentNode.contentDocument)
                this.treeOutline.panel.unregisterMutationEventListeners(currentNode.contentDocument.defaultView);
        }

        if (this.representedObject.contentDocument)
            updateChildrenOfNode(this.representedObject.contentDocument);
        updateChildrenOfNode(this.representedObject);

        var lastChild = this.children[this.children.length - 1];
        if (this.representedObject.nodeType == Node.ELEMENT_NODE && (!lastChild || !lastChild.elementCloseTag)) {
            var title = "<span class=\"webkit-html-tag close\">&lt;/" + this.representedObject.nodeName.toLowerCase().escapeHTML() + "&gt;</span>";
            var item = new TreeElement(title, null, false);
            item.selectable = false;
            item.elementCloseTag = true;
            this.appendChild(item);
        }
    },

    onexpand: function()
    {
        this.treeOutline.updateSelection();

        if (this.treeOutline.panel && this.representedObject.contentDocument)
            this.treeOutline.panel.registerMutationEventListeners(this.representedObject.contentDocument.defaultView);
    },

    oncollapse: function()
    {
        this.treeOutline.updateSelection();
    },

    onreveal: function()
    {
        if (this.listItemElement)
            this.listItemElement.scrollIntoViewIfNeeded(false);
    },

    onselect: function()
    {
        this.treeOutline.focusedDOMNode = this.representedObject;
        this.updateSelection();
    },

    onmousedown: function(event)
    {
        if (this._editing)
            return;

        // Prevent selecting the nearest word on double click.
        if (event.detail >= 2)
            event.preventDefault();
    },

    ondblclick: function(treeElement, event)
    {
        if (this._editing)
            return;

        if (this._startEditing(event))
            return;

        if (this.treeOutline.panel) {
            this.treeOutline.rootDOMNode = this.parent.representedObject;
            this.treeOutline.focusedDOMNode = this.representedObject;
        }

        if (this.hasChildren && !this.expanded)
            this.expand();
    },

    _startEditing: function(event)
    {
        if (this.treeOutline.focusedDOMNode != this.representedObject)
            return;

        if (this.representedObject.nodeType != Node.ELEMENT_NODE && this.representedObject.nodeType != Node.TEXT_NODE)
            return false;

        var textNode = event.target.enclosingNodeOrSelfWithClass("webkit-html-text-node");
        if (textNode)
            return this._startEditingTextNode(textNode);

        var attribute = event.target.enclosingNodeOrSelfWithClass("webkit-html-attribute");
        if (attribute)
            return this._startEditingAttribute(attribute, event);

        return false;
    },

    _startEditingAttribute: function(attribute, event)
    {
        if (WebInspector.isBeingEdited(attribute))
            return true;

        var attributeNameElement = attribute.getElementsByClassName("webkit-html-attribute-name")[0];
        if (!attributeNameElement)
            return false;

        var attributeName = attributeNameElement.innerText;

        function removeZeroWidthSpaceRecursive(node)
        {
            if (node.nodeType === Node.TEXT_NODE) {
                node.nodeValue = node.nodeValue.replace(/\u200B/g, "");
                return;
            }

            if (node.nodeType !== Node.ELEMENT_NODE)
                return;

            for (var child = node.firstChild; child; child = child.nextSibling)
                removeZeroWidthSpaceRecursive(child);
        }

        // Remove zero-width spaces that were added by nodeTitleInfo.
        removeZeroWidthSpaceRecursive(attribute);

        this._editing = true;

        WebInspector.startEditing(attribute, this._attributeEditingCommitted.bind(this), this._editingCancelled.bind(this), attributeName);
        window.getSelection().setBaseAndExtent(event.target, 0, event.target, 1);

        return true;
    },

    _startEditingTextNode: function(textNode)
    {
        if (WebInspector.isBeingEdited(textNode))
            return true;

        this._editing = true;

        WebInspector.startEditing(textNode, this._textNodeEditingCommitted.bind(this), this._editingCancelled.bind(this));
        window.getSelection().setBaseAndExtent(textNode, 0, textNode, 1);

        return true;
    },

    _attributeEditingCommitted: function(element, newText, oldText, attributeName)
    {
        delete this._editing;

        var parseContainerElement = document.createElement("span");
        parseContainerElement.innerHTML = "<span " + newText + "></span>";
        var parseElement = parseContainerElement.firstChild;
        if (!parseElement || !parseElement.hasAttributes()) {
            editingCancelled(element, context);
            return;
        }

        var foundOriginalAttribute = false;
        for (var i = 0; i < parseElement.attributes.length; ++i) {
            var attr = parseElement.attributes[i];
            foundOriginalAttribute = foundOriginalAttribute || attr.name === attributeName;
            InspectorController.inspectedWindow().Element.prototype.setAttribute.call(this.representedObject, attr.name, attr.value);
        }

        if (!foundOriginalAttribute)
            InspectorController.inspectedWindow().Element.prototype.removeAttribute.call(this.representedObject, attributeName);

        this._updateTitle();

        this.treeOutline.focusedNodeChanged(true);
    },

    _textNodeEditingCommitted: function(element, newText)
    {
        delete this._editing;

        var textNode;
        if (this.representedObject.nodeType == Node.ELEMENT_NODE) {
            // We only show text nodes inline in elements if the element only
            // has a single child, and that child is a text node.
            textNode = this.representedObject.firstChild;
        } else if (this.representedObject.nodeType == Node.TEXT_NODE)
            textNode = this.representedObject;

        textNode.nodeValue = newText;
        this._updateTitle();
    },

    _editingCancelled: function(element, context)
    {
        delete this._editing;

        this._updateTitle();
    },

    _updateTitle: function()
    {
        this.title = nodeTitleInfo.call(this.representedObject, this.hasChildren, WebInspector.linkifyURL).title;
        delete this.selectionElement;
        this.updateSelection();
        this._preventFollowingLinksOnDoubleClick();
    },
}

WebInspector.ElementsTreeElement.prototype.__proto__ = TreeElement.prototype;
