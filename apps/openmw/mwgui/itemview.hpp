#ifndef MWGUI_ITEMVIEW_H
#define MWGUI_ITEMVIEW_H

#include <MyGUI_Widget.h>

#include "itemmodel.hpp"
#include "itemlistwidget.hpp"
#include "itemlistwidgetheader.hpp"

namespace MWGui
{
    class ItemListWidgetHeader; 
    class ItemView final : public MyGUI::Widget
    {
    MYGUI_RTTI_DERIVED(ItemView)
    public:
        ItemView();
        virtual ~ItemView();

        /// Register needed components with MyGUI's factory manager
        static void registerComponents ();

        /// Takes ownership of \a model
        void setModel (ItemModel* model);
        ItemModel* getModel() { return mModel; }

        ItemListWidgetHeader* getHeader() { return mHeader; }

        /// Attempts to focus the selected index, will always be in bounds 
        /// returns the actual index focused 
        int forceItemFocused(int index);

        void disableHeader(bool disable) { mDisableHeader = true; }

        int requestListSize() const;

        typedef MyGUI::delegates::CMultiDelegate2<MyGUI::Widget*,MyGUI::KeyCode> EventHandle_WidgetKey;
        typedef MyGUI::delegates::CMultiDelegate1<ItemModel::ModelIndex> EventHandle_ModelIndex;
        typedef MyGUI::delegates::CMultiDelegate0 EventHandle_Void;
        typedef MyGUI::delegates::CMultiDelegate1<ItemListWidget*> EventHandle_Item;
        /// Fired when an item was clicked
        EventHandle_ModelIndex eventItemClicked;
        /// Fired when a key is pressed over a focused item 
        EventHandle_WidgetKey eventKeyButtonPressed;        
        /// Fired when the background was clicked (useful for drag and drop)
        EventHandle_Void eventBackgroundClicked;
        /// Fired when an icon gets mouse focus 
        EventHandle_Item eventItemFocused; 

        void update(bool force = false);

        void resetScrollBars();

    private:
        void initialiseOverride() final;

        void layoutWidgets();

        void setSize(const MyGUI::IntSize& _value) final;
        void setCoord(const MyGUI::IntCoord& _value) final;

        void onKeyButtonPressed(MyGUI::Widget *sender, MyGUI::KeyCode key, MyGUI::Char character);
        void onSelectedItem (MyGUI::Widget* sender);
        void onSelectedBackground (MyGUI::Widget* sender);
        void onMouseWheelMoved(MyGUI::Widget* _sender, int _rel);
        void onItemFocused(ItemListWidget* item);
        
        ItemModel* mModel;
        ItemModel::ModelIndex mLastIndex;
        
        MyGUI::ScrollView* mScrollView;
        ItemListWidgetHeader* mHeader;

        bool mDisableHeader;

    };

}

#endif
