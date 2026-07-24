/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EP_WINDOW_MESSAGE_H
#define EP_WINDOW_MESSAGE_H

// Headers
#include <string>
#include <vector>
#include "window_gold.h"
#include "window_numberinput.h"
#include "window_selectable.h"
#include "pending_message.h"
#include "async_op.h"
#include "text.h"

/**
 * Window Message Class.
 * This class displays the message boxes from
 * ShowMessageBox command code.
 */
class Window_Message: public Window_Selectable {
public:
	Window_Message(int ix, int iy, int iwidth, int iheight);
	~Window_Message() override;

	enum WindowMessageValues {
		LeftMargin = 8,
		FaceSize = 48,
		RightFaceMargin = 16,
		TopMargin = 8
	};

	/**
	 * Starts message processing by reading all
	 * non-displayed from Game_Message.
	 */
	void StartMessageProcessing(PendingMessage pm);

	/**
	 * Ends the message processing.
	 */
	void FinishMessageProcessing();

	/**
	 * Does the initial steps to start a choice selection.
	 */
	void StartChoiceProcessing();

	/**
	 * Does the initial steps to start a number input.
	 */
	void StartNumberInputProcessing();

	/**
	 * Shows the Gold Window
	 */
	void ShowGoldWindow();

	/**
	 * Clears the Messagebox and places the write pointer
	 * in the top left corner.
	 */
	void InsertNewPage();

	/**
	 * Inserts a line break.
	 */
	void InsertNewLine();

	void OnFinishPage();

	/**
	 * Stub.
	 */
	void ResetWindow();

	void Update() override;

	/**
	 * Continues outputting more text. Also handles the
	 * CommandCode parsing.
	 */
	virtual void UpdateMessage();

	/**
	 * Stub. For choice.
	 */
	void UpdateCursorRect() override;

	/**
	 * Waits for a key press before the text output
	 * continutes.
	 */
	void WaitForInput();

	/**
	 * Stub. Handles choice selection.
	 */
	void InputChoice();

	/**
	 * Handles number input.
	 */
	void InputNumber();

	/** @return the stored PendingMessage */
	const PendingMessage& GetPendingMessage() const;

	/** @return true if there is still text pending */
	bool IsMessagePending() const;

	/** @return true if we can push a new message this frame */
	bool GetAllowNextMessage(bool foreground) const;

	/** @return the last async operation */
	AsyncOp GetAsyncOp() const;

	/** @return the number of lines per page */
	int GetMaxLinesPerPage() const;

	/**
	 * Set the number of lines per page
	 *
	 * @param lines the number of lines
	 **/
	void SetMaxLinesPerPage(int lines);

	/** @return the current text alignment used for dialogue lines */
	Text::Alignment GetTextAlignment() const;

	/**
	 * Set the horizontal alignment used to place each dialogue line
	 * inside the message window (Left / Center / Right).
	 *
	 * @param align the new alignment
	 */
	void SetTextAlignment(Text::Alignment align);

protected:
	/** Async operation */
	AsyncOp aop;
	/** X-position of next char. */
	int contents_x = 0;
	/** Y-position of next char. */
	int contents_y = 0;
	/** Current number of lines on this page. */
	int line_count = 0;
	/** Maximum number of lines per page */
	int max_lines_per_page = 4;
	/** Index of the next char in text that will be output. */
	const char* text_index = nullptr;
	/** text message that will be displayed. */
	std::string text;
	/** Text color. */
	int text_color = 0;
	/** Current speed modifier. */
	int speed = 1;
	/** Used by Message kill command \^. */
	bool kill_page = false;
	/** If instant speed is enabled */
	bool instant_speed = false;

	// FIXME: This hacky flags exist because RPG_RT likely animates the message window
	// after the game loop finishes. Our code isn't structured that way, so we must hack
	// around it.
	bool close_started_this_frame = false;
	bool close_finished_this_frame = false;

	/** Frames to wait when a message wait command was used */
	int wait_count = 0;

	/** Incremented by 1 each time we print a half width character with speed 1,
	 * or by 2 for any other character */
	int line_char_counter = 0;

	/** Did we wait for the prev character */
	bool prev_char_waited = true;
	/** Was the previous character printable? */
	bool prev_char_printable = false;
	/** Active font for the current page */
	FontRef page_font;

	/** Used by the number input event. */
	std::unique_ptr<Window_NumberInput> number_input_window;
	std::unique_ptr<Window_Gold> gold_window;

	PendingMessage pending_message;

	std::vector<Font::ShapeRet> shape_ret;

	/** Horizontal alignment applied to every dialogue line. Defaults to right-aligned
	 * so that text sticks to the right edge of the window, as requested. */
	Text::Alignment text_align = Text::AlignRight;

	/** Pre-computed pixel width of each line in the current message, used to
	 * position the line according to text_align before typing starts. */
	std::vector<int> line_widths;
	/** Index into line_widths of the line that is about to be typed. */
	size_t line_widths_index = 0;

	/** Left-most X coordinate available for text on the current page (accounts for
	 * a left-positioned face graphic and/or choice indentation). */
	int text_left_bound = 0;
	/** Right-most X coordinate available for text on the current page (accounts for
	 * a right-positioned face graphic). */
	int text_right_bound = 0;

	bool DrawGlyph(Font& font, const Bitmap& system, char32_t glyph, bool is_exfont);
	bool DrawGlyph(Font& font, const Bitmap& system, const Font::ShapeRet& shape);
	void IncrementLineCharCounter(int width);

	void SetWaitForCharacter(int width);
	void SetWaitForNonPrintable(int frames);
	void SetWait(int frames);

	/** @return the starting X position for a line of the given pixel width,
	 * honoring text_align and the current text_left_bound / text_right_bound. */
	int ComputeLineStartX(int line_width) const;

	bool IsFaceEnabled() const;
};

inline const PendingMessage& Window_Message::GetPendingMessage() const {
	return pending_message;
}

inline bool Window_Message::IsMessagePending() const {
	return IsVisible() && !IsClosing();
}

inline AsyncOp Window_Message::GetAsyncOp() const {
	return aop;
}

inline bool Window_Message::GetAllowNextMessage(bool foreground) const {
	bool is_active = (IsVisible() || close_finished_this_frame);
	return foreground ? !is_active || close_started_this_frame : !is_active;
}

inline int Window_Message::GetMaxLinesPerPage() const {
	return max_lines_per_page;
}

/** @return the number of lines per page */
inline void Window_Message::SetMaxLinesPerPage(int lines) {
	max_lines_per_page = lines;
}

inline Text::Alignment Window_Message::GetTextAlignment() const {
	return text_align;
}

inline void Window_Message::SetTextAlignment(Text::Alignment align) {
	text_align = align;
}

#endif
