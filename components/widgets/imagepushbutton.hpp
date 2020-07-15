#ifndef OPENMW_COMPONENTS_WIDGETS_IMAGEPUSHBUTTON_H
#define OPENMW_COMPONENTS_WIDGETS_IMAGEPUSHBUTTON_H

#include <MyGUI_ImageBox.h>

namespace Gui
{

    /**
     * @brief allows using different image textures depending on the button state
     */
    class ImagePushButton final : public MyGUI::ImageBox
    {
        MYGUI_RTTI_DERIVED(ImagePushButton)

    public:
        MyGUI::IntSize getRequestedSize();

        ImagePushButton();

        static void setDefaultNeedKeyFocus(bool enabled);

        void setStateSelected(bool value);

        bool getStateSelected() const;

        /// Set mImageNormal, mImageHighlighted and mImagePushed based on file convention (image_idle.ext, image_over.ext and image_pressed.ext)
        void setImage(const std::string& image);

        void setTextureRect(MyGUI::IntCoord coord);

    private:
        void updateImage();

        static bool sDefaultNeedKeyFocus;

    protected:
        void setPropertyOverride(const std::string& _key, const std::string& _value) final;
        void onMouseLostFocus(MyGUI::Widget* _new) final;
        void onMouseSetFocus(MyGUI::Widget* _old) final;
        void onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id) final;
        void onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id) final;
        void onKeySetFocus(MyGUI::Widget* _old) final;
        void onKeyLostFocus(MyGUI::Widget* _new) final;

        std::string mImageHighlighted;
        std::string mImageNormal;
        std::string mImagePushed;

        bool mMouseFocus;
        bool mMousePress;
        bool mKeyFocus;
        bool mUseWholeTexture;
        bool mSelected;

        MyGUI::IntCoord mTextureRect;
    };

}

#endif
