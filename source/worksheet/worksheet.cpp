// Copyright (c) 2014-2017 Thomas Fussell
// Copyright (c) 2010-2015 openpyxl
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, WRISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
// @license: http://www.opensource.org/licenses/mit-license.php
// @author: see AUTHORS file

#include <algorithm>
#include <cmath>
#include <limits>

#include <detail/constants.hpp>
#include <detail/implementations/cell_impl.hpp>
#include <detail/implementations/workbook_impl.hpp>
#include <detail/implementations/worksheet_impl.hpp>
#include <xlnt/cell/cell.hpp>
#include <xlnt/cell/cell_reference.hpp>
#include <xlnt/cell/index_types.hpp>
#include <xlnt/packaging/relationship.hpp>
#include <xlnt/utils/date.hpp>
#include <xlnt/utils/datetime.hpp>
#include <xlnt/utils/exceptions.hpp>
#include <xlnt/workbook/named_range.hpp>
#include <xlnt/workbook/workbook.hpp>
#include <xlnt/workbook/worksheet_iterator.hpp>
#include <xlnt/worksheet/cell_iterator.hpp>
#include <xlnt/worksheet/header_footer.hpp>
#include <xlnt/worksheet/range.hpp>
#include <xlnt/worksheet/range_iterator.hpp>
#include <xlnt/worksheet/range_reference.hpp>
#include <xlnt/worksheet/worksheet.hpp>

namespace {

int points_to_pixels(double points, double dpi)
{
    return static_cast<int>(std::ceil(points * dpi / 72));
}

} // namespace

namespace xlnt {

worksheet::worksheet()
    : d_(nullptr)
{
}

worksheet::worksheet(detail::worksheet_impl *d)
    : d_(d)
{
}

worksheet::worksheet(const worksheet &rhs)
    : d_(rhs.d_)
{
}


///////////////////////


#if 0
/*
 * Set up image/drawings.
 */
void
lxw_worksheet_prepare_image(lxw_worksheet *self,
                            uint16_t image_ref_id, uint16_t drawing_id,
                            lxw_image_options *image_data)
{
    lxw_drawing_object *drawing_object;
    lxw_rel_tuple *relationship;
    double width;
    double height;
    char filename[LXW_FILENAME_LENGTH];

    if (!self->drawing) {
        self->drawing = lxw_drawing_new();
        self->drawing->embedded = LXW_TRUE;
        RETURN_VOID_ON_MEM_ERROR(self->drawing);

        relationship = calloc(1, sizeof(lxw_rel_tuple));
        GOTO_LABEL_ON_MEM_ERROR(relationship, mem_error);

        relationship->type = lxw_strdup("/drawing");
        GOTO_LABEL_ON_MEM_ERROR(relationship->type, mem_error);

        lxw_snprintf(filename, LXW_FILENAME_LENGTH,
                     "../drawings/drawing%d.xml", drawing_id);

        relationship->target = lxw_strdup(filename);
        GOTO_LABEL_ON_MEM_ERROR(relationship->target, mem_error);

        STAILQ_INSERT_TAIL(self->external_drawing_links, relationship,
                           list_pointers);
    }

    drawing_object = calloc(1, sizeof(lxw_drawing_object));
    RETURN_VOID_ON_MEM_ERROR(drawing_object);

    drawing_object->anchor_type = LXW_ANCHOR_TYPE_IMAGE;
    drawing_object->edit_as = LXW_ANCHOR_EDIT_AS_ONE_CELL;
    drawing_object->description = lxw_strdup(image_data->short_name);

    /* Scale to user scale. */
    width = image_data->width * image_data->x_scale;
    height = image_data->height * image_data->y_scale;

    /* Scale by non 96dpi resolutions. */
    width *= 96.0 / image_data->x_dpi;
    height *= 96.0 / image_data->y_dpi;

    /* Convert to the nearest pixel. */
    image_data->width = width;
    image_data->height = height;

    _worksheet_position_object_emus(self, image_data, drawing_object);

    /* Convert from pixels to emus. */
    drawing_object->width = (uint32_t) (0.5 + width * 9525);
    drawing_object->height = (uint32_t) (0.5 + height * 9525);

    lxw_add_drawing_object(self->drawing, drawing_object);

    relationship = calloc(1, sizeof(lxw_rel_tuple));
    GOTO_LABEL_ON_MEM_ERROR(relationship, mem_error);

    relationship->type = lxw_strdup("/image");
    GOTO_LABEL_ON_MEM_ERROR(relationship->type, mem_error);

    lxw_snprintf(filename, 32, "../media/image%d.%s", image_ref_id,
                 image_data->extension);

    relationship->target = lxw_strdup(filename);
    GOTO_LABEL_ON_MEM_ERROR(relationship->target, mem_error);

    STAILQ_INSERT_TAIL(self->drawing_links, relationship, list_pointers);

    return;

mem_error:
    if (relationship) {
        free(relationship->type);
        free(relationship->target);
        free(relationship->target_mode);
        free(relationship);
    }
}

#if 0
/*
 * Set up chart/drawings.
 */
void
lxw_worksheet_prepare_chart(lxw_worksheet *self,
                            uint16_t chart_ref_id, uint16_t drawing_id,
                            lxw_image_options *image_data)
{
    lxw_drawing_object *drawing_object;
    lxw_rel_tuple *relationship;
    double width;
    double height;
    char filename[LXW_FILENAME_LENGTH];

    if (!self->drawing) {
        self->drawing = lxw_drawing_new();
        self->drawing->embedded = LXW_TRUE;
        RETURN_VOID_ON_MEM_ERROR(self->drawing);

        relationship = calloc(1, sizeof(lxw_rel_tuple));
        GOTO_LABEL_ON_MEM_ERROR(relationship, mem_error);

        relationship->type = lxw_strdup("/drawing");
        GOTO_LABEL_ON_MEM_ERROR(relationship->type, mem_error);

        lxw_snprintf(filename, LXW_FILENAME_LENGTH,
                     "../drawings/drawing%d.xml", drawing_id);

        relationship->target = lxw_strdup(filename);
        GOTO_LABEL_ON_MEM_ERROR(relationship->target, mem_error);

        STAILQ_INSERT_TAIL(self->external_drawing_links, relationship,
                           list_pointers);
    }

    drawing_object = calloc(1, sizeof(lxw_drawing_object));
    RETURN_VOID_ON_MEM_ERROR(drawing_object);

    drawing_object->anchor_type = LXW_ANCHOR_TYPE_CHART;
    drawing_object->edit_as = LXW_ANCHOR_EDIT_AS_ONE_CELL;
    drawing_object->description = lxw_strdup("TODO_DESC");

    /* Scale to user scale. */
    width = image_data->width * image_data->x_scale;
    height = image_data->height * image_data->y_scale;

    /* Convert to the nearest pixel. */
    image_data->width = width;
    image_data->height = height;

    _worksheet_position_object_emus(self, image_data, drawing_object);

    /* Convert from pixels to emus. */
    drawing_object->width = (uint32_t) (0.5 + width * 9525);
    drawing_object->height = (uint32_t) (0.5 + height * 9525);

    lxw_add_drawing_object(self->drawing, drawing_object);

    relationship = calloc(1, sizeof(lxw_rel_tuple));
    GOTO_LABEL_ON_MEM_ERROR(relationship, mem_error);

    relationship->type = lxw_strdup("/chart");
    GOTO_LABEL_ON_MEM_ERROR(relationship->type, mem_error);

    lxw_snprintf(filename, 32, "../charts/chart%d.xml", chart_ref_id);

    relationship->target = lxw_strdup(filename);
    GOTO_LABEL_ON_MEM_ERROR(relationship->target, mem_error);

    STAILQ_INSERT_TAIL(self->drawing_links, relationship, list_pointers);

    return;

mem_error:
    if (relationship) {
        free(relationship->type);
        free(relationship->target);
        free(relationship->target_mode);
        free(relationship);
    }
}

#endif




/*
 * Extract width and height information from a PNG file.
 */
static bool _process_png(ImageOptions *image_options)
{
    uint32_t length;
    uint32_t offset;
    char type[4];
    uint32_t width = 0;
    uint32_t height = 0;
    double x_dpi = 96;
    double y_dpi = 96;
    int fseek_err;

    FILE *stream = image_options->stream;

    /* Skip another 4 bytes to the end of the PNG header. */
    fseek_err = fseek(stream, 4, SEEK_CUR);
    if (fseek_err)
        goto file_error;

    while (!feof(stream)) 
    {

        /* Read the PNG length and type fields for the sub-section. */
        if (fread(&length, sizeof(length), 1, stream) < 1)
            break;

        if (fread(&type, 1, 4, stream) < 4)
            break;

        /* Convert the length to network order. */
        length = UINT32_NETWORK(length);

        /* The offset for next fseek() is the field length + type length. */
        offset = length + 4;

        if (memcmp(type, "IHDR", 4) == 0) 
        {
            if (fread(&width, sizeof(width), 1, stream) < 1)
                break;

            if (fread(&height, sizeof(height), 1, stream) < 1)
                break;

            width = UINT32_NETWORK(width);
            height = UINT32_NETWORK(height);

            /* Reduce the offset by the length of previous freads(). */
            offset -= 8;
        }

        if (memcmp(type, "pHYs", 4) == 0) 
        {
            uint32_t x_ppu = 0;
            uint32_t y_ppu = 0;
            uint8_t units = 1;

            if (fread(&x_ppu, sizeof(x_ppu), 1, stream) < 1)
                break;

            if (fread(&y_ppu, sizeof(y_ppu), 1, stream) < 1)
                break;

            if (fread(&units, sizeof(units), 1, stream) < 1)
                break;

            if (units == 1) {
                x_ppu = UINT32_NETWORK(x_ppu);
                y_ppu = UINT32_NETWORK(y_ppu);

                x_dpi = (double) x_ppu *0.0254;
                y_dpi = (double) y_ppu *0.0254;
            }

            /* Reduce the offset by the length of previous freads(). */
            offset -= 9;
        }

        if (memcmp(type, "IEND", 4) == 0)
            break;

        if (!feof(stream)) 
        {
            fseek_err = fseek(stream, offset, SEEK_CUR);
            if (fseek_err)
                goto file_error;
        }
    }

    /* Ensure that we read some valid data from the file. */
    if (width == 0)
        goto file_error;

    /* Set the image metadata. */
    image_options->image_type = cell::ImageType::IMAGE_PNG;
    image_options->width = width;
    image_options->height = height;
    image_options->x_dpi = x_dpi ? x_dpi : 96;
    image_options->y_dpi = y_dpi ? x_dpi : 96;
    image_options->extension = strdup("png");

    return true;

file_error:
    //LXW_WARN_FORMAT1("worksheet_insert_image()/_opt(): "
    //                 "no size data found in file: %s.",
    //                 image_options->filename);

    return false;
}

/*
 * Extract width and height information from a JPEG file.
 */
static bool _process_jpeg(ImageOptions *image_options)
{
    uint16_t length;
    uint16_t marker;
    uint32_t offset;
    uint16_t width = 0;
    uint16_t height = 0;
    double x_dpi = 96;
    double y_dpi = 96;
    int fseek_err;

    FILE *stream = image_options->stream;

    /* Read back 2 bytes to the end of the initial 0xFFD8 marker. */
    fseek_err = fseek(stream, -2, SEEK_CUR);
    if (fseek_err)
        goto file_error;

    /* Search through the image data to read the height and width in the */
    /* 0xFFC0/C2 element. Also read the DPI in the 0xFFE0 element. */
    while (!feof(stream)) 
    {

        /* Read the JPEG marker and length fields for the sub-section. */
        if (fread(&marker, sizeof(marker), 1, stream) < 1)
            break;

        if (fread(&length, sizeof(length), 1, stream) < 1)
            break;

        /* Convert the marker and length to network order. */
        marker = UINT16_NETWORK(marker);
        length = UINT16_NETWORK(length);

        /* The offset for next fseek() is the field length + type length. */
        offset = length - 2;

        if (marker == 0xFFC0 || marker == 0xFFC2) 
        {
            /* Skip 1 byte to height and width. */
            fseek_err = fseek(stream, 1, SEEK_CUR);
            if (fseek_err)
                goto file_error;

            if (fread(&height, sizeof(height), 1, stream) < 1)
                break;

            if (fread(&width, sizeof(width), 1, stream) < 1)
                break;

            height = UINT16_NETWORK(height);
            width = UINT16_NETWORK(width);

            offset -= 9;
        }

        if (marker == 0xFFE0) 
        {
            uint16_t x_density = 0;
            uint16_t y_density = 0;
            uint8_t units = 1;

            fseek_err = fseek(stream, 7, SEEK_CUR);
            if (fseek_err)
                goto file_error;

            if (fread(&units, sizeof(units), 1, stream) < 1)
                break;

            if (fread(&x_density, sizeof(x_density), 1, stream) < 1)
                break;

            if (fread(&y_density, sizeof(y_density), 1, stream) < 1)
                break;

            x_density = UINT16_NETWORK(x_density);
            y_density = UINT16_NETWORK(y_density);

            if (units == 1) {
                x_dpi = x_density;
                y_dpi = y_density;
            }

            if (units == 2) 
            {
                x_dpi = x_density * 2.54;
                y_dpi = y_density * 2.54;
            }

            offset -= 12;
        }

        if (marker == 0xFFDA)
            break;

        if (!feof(stream)) 
        {
            fseek_err = fseek(stream, offset, SEEK_CUR);
            if (fseek_err)
                goto file_error;
        }
    }

    /* Ensure that we read some valid data from the file. */
    if (width == 0)
        goto file_error;

    /* Set the image metadata. */
    image_options->image_type = cell::ImageType::IMAGE_JPEG;
    image_options->width = width;
    image_options->height = height;
    image_options->x_dpi = x_dpi ? x_dpi : 96;
    image_options->y_dpi = y_dpi ? x_dpi : 96;
    image_options->extension = strdup("jpeg");

    return true;

file_error:
    //LXW_WARN_FORMAT1("worksheet_insert_image()/_opt(): "
    //                 "no size data found in file: %s.",
    //                 image_options->filename);

    return false;
}

/*
 * Extract width and height information from a BMP file.
 */
static bool _process_bmp(ImageOptions *image_options)
{
    uint32_t width = 0;
    uint32_t height = 0;
    double x_dpi = 96;
    double y_dpi = 96;
    int fseek_err;

    FILE *stream = image_options->stream;

    /* Skip another 14 bytes to the start of the BMP height/width. */
    fseek_err = fseek(stream, 14, SEEK_CUR);
    if (fseek_err)
        goto file_error;

    if (fread(&width, sizeof(width), 1, stream) < 1)
        width = 0;

    if (fread(&height, sizeof(height), 1, stream) < 1)
        height = 0;

    /* Ensure that we read some valid data from the file. */
    if (width == 0)
        goto file_error;

    /* Set the image metadata. */
    image_options->image_type = cell::ImageType::IMAGE_BMP;
    image_options->width = width;
    image_options->height = height;
    image_options->x_dpi = x_dpi;
    image_options->y_dpi = y_dpi;
    image_options->extension = strdup("bmp");

    return true;

file_error:
    //LXW_WARN_FORMAT1("worksheet_insert_image()/_opt(): "
    //                 "no size data found in file: %s.",
     //                image_options->filename);

    return false;
}




static bool _get_image_properties(ImageOptions *image_options)
{
    unsigned char signature[4];

    /* Read 4 bytes to look for the file header/signature. */
    if (fread(signature, 1, 4, image_options->stream) < 4) 
    {
        //LXW_WARN_FORMAT1("worksheet_insert_image()/_opt(): "
        //                 "couldn't read file type for file: %s.",
        //                 image_options->filename);
        return false;
    }

    if (memcmp(&signature[1], "PNG", 3) == 0) 
    {
        if (!_process_png(image_options))
            return false;
    }
    else if (signature[0] == 0xFF && signature[1] == 0xD8) 
    {
        if (!_process_jpeg(image_options))
            return false;
    }
    else if (memcmp(signature, "BM", 2) == 0) 
    {
        if (!_process_bmp(image_options))
            return false;
    }
    else 
    {
        //LXW_WARN_FORMAT1("worksheet_insert_image()/_opt(): "
        //                 "unsupported image format for file: %s.",
        //                 image_options->filename);
        return false;
    }

    return true;
}




/*
 * Simple replacement for libgen.h basename() for compatibility with MSVC. It
 * handles forward and back slashes. It doesn't copy exactly the return
 * format of basename().
 */
static char* lxw_basename(const char *path)
{

    char *forward_slash;
    char *back_slash;

    if (!path)
        return NULL;

    forward_slash = strrchr(path, '/');
    back_slash = strrchr(path, '\\');

    if (!forward_slash && !back_slash)
        return (char *) path;

    if (forward_slash > back_slash)
        return forward_slash + 1;
    else
        return back_slash + 1;
}
#endif


//
// Insert an image into the worksheet.
// Image data is not written in the worksheet section, per se. 
// The data is written later in the workbook section, then referenced back by the worksheet. 
// The image doesn’t appear to belong to cell, but instead get a position on the screen.
//
// Here we add the reference information to the image, leaving it to the builder to add the
// actual image bytes to the file.
//
Image& worksheet::addImage(const Image& img)
{
    Image image (img);

    image.id = workbook().addMedia(image.imagePath);
    drawings_[image.id] = image;

    return static_cast<Image&>(drawings_[image.id]);
}









///////////////////////




bool worksheet::has_frozen_panes() const
{
    return !d_->views_.empty() && d_->views_.front().has_pane()
        && (d_->views_.front().pane().state == pane_state::frozen
               || d_->views_.front().pane().state == pane_state::frozen_split);
}

void worksheet::create_named_range(const std::string &name, const std::string &reference_string)
{
    create_named_range(name, range_reference(reference_string));
}

void worksheet::create_named_range(const std::string &name, const range_reference &reference)
{
    try
    {
        auto temp = cell_reference::split_reference(name);

        // name is a valid reference, make sure it's outside the allowed range

        if (column_t(temp.first).index <= column_t("XFD").index && temp.second <= 1048576)
        {
            throw invalid_parameter(); //("named range name must be outside the range A1-XFD1048576");
        }
    }
    catch (xlnt::invalid_cell_reference)
    {
        // name is not a valid reference, that's good
    }

    std::vector<named_range::target> targets;
    targets.push_back({*this, reference});

    d_->named_ranges_[name] = xlnt::named_range(name, targets);
}

cell worksheet::operator[](const cell_reference &ref)
{
    return cell(ref);
}

std::vector<range_reference> worksheet::merged_ranges() const
{
    return d_->merged_cells_;
}

bool worksheet::has_page_margins() const
{
    return d_->page_margins_.is_set();
}

bool worksheet::has_page_setup() const
{
    return d_->page_setup_.is_set();
}

page_margins worksheet::page_margins() const
{
    return d_->page_margins_.get();
}

void worksheet::page_margins(const class page_margins &margins)
{
    d_->page_margins_ = margins;
}

void worksheet::auto_filter(const std::string &reference_string)
{
    auto_filter(range_reference(reference_string));
}

void worksheet::auto_filter(const range_reference &reference)
{
    d_->auto_filter_ = reference;
}

void worksheet::auto_filter(const xlnt::range &range)
{
    auto_filter(range.reference());
}

range_reference worksheet::auto_filter() const
{
    return d_->auto_filter_.get();
}

bool worksheet::has_auto_filter() const
{
    return d_->auto_filter_.is_set();
}

void worksheet::clear_auto_filter()
{
    d_->auto_filter_.clear();
}

void worksheet::page_setup(const struct page_setup &setup)
{
    d_->page_setup_ = setup;
}

page_setup worksheet::page_setup() const
{
    if (!has_page_setup())
    {
        throw invalid_attribute();
    }

    return d_->page_setup_.get();
}

workbook &worksheet::workbook()
{
    return *d_->parent_;
}

const workbook &worksheet::workbook() const
{
    return *d_->parent_;
}

void worksheet::garbage_collect()
{
    auto cell_map_iter = d_->cell_map_.begin();

    while (cell_map_iter != d_->cell_map_.end())
    {
        auto cell_iter = cell_map_iter->second.begin();

        while (cell_iter != cell_map_iter->second.end())
        {
            class cell current_cell(&cell_iter->second);

            if (current_cell.garbage_collectible())
            {
                cell_iter = cell_map_iter->second.erase(cell_iter);
                continue;
            }

            cell_iter++;
        }

        if (cell_map_iter->second.empty())
        {
            cell_map_iter = d_->cell_map_.erase(cell_map_iter);
            continue;
        }

        cell_map_iter++;
    }
}

void worksheet::id(std::size_t id)
{
    d_->id_ = id;
}

std::size_t worksheet::id() const
{
    return d_->id_;
}

std::string worksheet::title() const
{
    return d_->title_;
}

void worksheet::title(const std::string &title)
{
    if (title.length() > 31)
    {
        throw invalid_sheet_title(title);
    }

    if (title.find_first_of("*:/\\?[]") != std::string::npos)
    {
        throw invalid_sheet_title(title);
    }

    auto same_title = std::find_if(workbook().begin(), workbook().end(),
        [&](worksheet ws) { return ws.title() == title; });

    if (same_title != workbook().end() && *same_title != *this)
    {
        throw invalid_sheet_title(title);
    }

    workbook().d_->sheet_title_rel_id_map_[title] = workbook().d_->sheet_title_rel_id_map_[d_->title_];
    workbook().d_->sheet_title_rel_id_map_.erase(d_->title_);
    d_->title_ = title;

    workbook().update_sheet_properties();
}

cell_reference worksheet::frozen_panes() const
{
    if (!has_frozen_panes())
    {
        throw xlnt::invalid_attribute();
    }

    return d_->views_.front().pane().top_left_cell.get();
}

void worksheet::freeze_panes(xlnt::cell top_left_cell)
{
    freeze_panes(top_left_cell.reference());
}

void worksheet::freeze_panes(const cell_reference &ref)
{
    if (!has_view())
    {
        d_->views_.push_back(sheet_view());
    }

    auto &primary_view = d_->views_.front();

    if (!primary_view.has_pane())
    {
        primary_view.pane(pane());
    }

    primary_view.pane().top_left_cell = ref;
    primary_view.pane().state = pane_state::frozen;

    primary_view.clear_selections();
    primary_view.add_selection(selection());

    if (ref == "A1")
    {
        unfreeze_panes();
    }
    else if (ref.column() == "A")
    {
        primary_view.add_selection(selection());
        primary_view.selection(0).pane(pane_corner::bottom_left);
        primary_view.selection(0).active_cell(ref.make_offset(0, -1)); // cell above
        primary_view.selection(1).active_cell(ref);
        primary_view.pane().active_pane = pane_corner::bottom_left;
        primary_view.pane().y_split = ref.row() - 1;
    }
    else if (ref.row() == 1)
    {
        primary_view.add_selection(selection());
        primary_view.selection(0).pane(pane_corner::top_right);
        primary_view.selection(0).active_cell(ref.make_offset(-1, 0)); // cell to the left
        primary_view.selection(1).active_cell(ref);
        primary_view.pane().active_pane = pane_corner::top_right;
        primary_view.pane().x_split = ref.column_index() - 1;
    }
    else
    {
        primary_view.add_selection(selection());
        primary_view.add_selection(selection());
        primary_view.selection(0).pane(pane_corner::top_right);
        primary_view.selection(0).active_cell(ref.make_offset(0, -1)); // cell above
        primary_view.selection(1).pane(pane_corner::bottom_left);
        primary_view.selection(1).active_cell(ref.make_offset(-1, 0)); // cell to the left
        primary_view.selection(2).pane(pane_corner::bottom_right);
        primary_view.selection(2).active_cell(ref);
        primary_view.pane().active_pane = pane_corner::bottom_right;
        primary_view.pane().x_split = ref.column_index() - 1;
        primary_view.pane().y_split = ref.row() - 1;
    }
}

void worksheet::unfreeze_panes()
{
    if (!has_view()) return;

    auto &primary_view = d_->views_.front();

    primary_view.clear_selections();
    primary_view.clear_pane();
}

void worksheet::active_cell(const cell_reference &ref)
{
    if (!has_view())
    {
        d_->views_.push_back(sheet_view());
    }

    auto &primary_view = d_->views_.front();

    if (!primary_view.has_selections())
    {
        primary_view.add_selection(selection());
    }

    auto &primary_selection = primary_view.selection(0);
    primary_selection.active_cell(ref);
}

bool worksheet::has_active_cell() const
{
    if (!has_view()) return false;
    auto &primary_view = d_->views_.front();
    if (!primary_view.has_selections()) return false;
    auto primary_selection = primary_view.selection(0);

    return primary_selection.has_active_cell();
}

cell_reference worksheet::active_cell() const
{
    if (!has_view())
    {
        throw xlnt::exception("Worksheet has no view.");
    }

    auto &primary_view = d_->views_.front();

    if (!primary_view.has_selections())
    {
        throw xlnt::exception("Default worksheet view has no selections.");
    }

    return primary_view.selection(0).active_cell();
}

cell worksheet::cell(const cell_reference &reference)
{
    auto &row = d_->cell_map_[reference.row()];
    auto match = row.find(reference.column_index());

    if (match == row.end())
    {
        match = row.emplace(reference.column_index(), detail::cell_impl()).first;
        auto &impl = match->second;

        impl.parent_ = d_;
        impl.column_ = reference.column_index();
        impl.row_ = reference.row();
    }

    return xlnt::cell(&match->second);
}

const cell worksheet::cell(const cell_reference &reference) const
{
    return xlnt::cell(&d_->cell_map_.at(reference.row()).at(reference.column_index()));
}

cell worksheet::cell(xlnt::column_t column, row_t row)
{
    return cell(cell_reference(column, row));
}

const cell worksheet::cell(xlnt::column_t column, row_t row) const
{
    return cell(cell_reference(column, row));
}

bool worksheet::has_cell(const cell_reference &reference) const
{
    const auto row = d_->cell_map_.find(reference.row());
    if (row == d_->cell_map_.cend()) return false;

    const auto col = row->second.find(reference.column_index());
    if (col == row->second.cend()) return false;

    return true;
}

bool worksheet::has_row_properties(row_t row) const
{
    return d_->row_properties_.find(row) != d_->row_properties_.end();
}

range worksheet::named_range(const std::string &name)
{
    if (!workbook().has_named_range(name))
    {
        throw key_not_found();
    }

    if (!has_named_range(name))
    {
        throw key_not_found();
    }

    return range(d_->named_ranges_[name].targets()[0].second);
}

const range worksheet::named_range(const std::string &name) const
{
    if (!workbook().has_named_range(name))
    {
        throw key_not_found();
    }

    if (!has_named_range(name))
    {
        throw key_not_found();
    }

    return range(d_->named_ranges_[name].targets()[0].second);
}

column_t worksheet::lowest_column() const
{
    if (d_->cell_map_.empty())
    {
        return constants::min_column();
    }

    auto lowest = constants::max_column();

    for (auto &row : d_->cell_map_)
    {
        for (auto &c : row.second)
        {
            lowest = std::min(lowest, c.first);
        }
    }

    return lowest;
}

column_t worksheet::lowest_column_or_props() const
{
    auto lowest = lowest_column();

    if (d_->cell_map_.empty() && !d_->column_properties_.empty())
    {
        lowest = d_->column_properties_.begin()->first;
    }

    for (auto &props : d_->column_properties_)
    {
        lowest = std::min(lowest, props.first);
    }

    return lowest;
}

row_t worksheet::lowest_row() const
{
    if (d_->cell_map_.empty())
    {
        return constants::min_row();
    }

    auto lowest = constants::max_row();

    for (auto &row : d_->cell_map_)
    {
        lowest = std::min(lowest, row.first);
    }

    return lowest;
}

row_t worksheet::lowest_row_or_props() const
{
    auto lowest = lowest_row();

    if (d_->cell_map_.empty() && !d_->row_properties_.empty())
    {
        lowest = d_->row_properties_.begin()->first;
    }

    for (auto &props : d_->row_properties_)
    {
        lowest = std::min(lowest, props.first);
    }

    return lowest;
}

row_t worksheet::highest_row() const
{
    auto highest = constants::min_row();

    for (auto &row : d_->cell_map_)
    {
        highest = std::max(highest, row.first);
    }

    return highest;
}

row_t worksheet::highest_row_or_props() const
{
    auto highest = highest_row();

    if (d_->cell_map_.empty() && !d_->row_properties_.empty())
    {
        highest = d_->row_properties_.begin()->first;
    }

    for (auto &props : d_->row_properties_)
    {
        highest = std::max(highest, props.first);
    }

    return highest;
}

column_t worksheet::highest_column() const
{
    auto highest = constants::min_column();

    for (auto &row : d_->cell_map_)
    {
        for (auto &c : row.second)
        {
            highest = std::max(highest, c.first);
        }
    }

    return highest;
}

column_t worksheet::highest_column_or_props() const
{
    auto highest = highest_column();

    if (d_->cell_map_.empty() && !d_->column_properties_.empty())
    {
        highest = d_->column_properties_.begin()->first;
    }

    for (auto &props : d_->column_properties_)
    {
        highest = std::max(highest, props.first);
    }

    return highest;
}

range_reference worksheet::calculate_dimension() const
{
    return range_reference(lowest_column(), lowest_row(), highest_column(), highest_row());
}

range worksheet::range(const std::string &reference_string)
{
    if (has_named_range(reference_string))
    {
        return named_range(reference_string);
    }

    return range(range_reference(reference_string));
}

const range worksheet::range(const std::string &reference_string) const
{
    if (has_named_range(reference_string))
    {
        return named_range(reference_string);
    }

    return range(range_reference(reference_string));
}

range worksheet::range(const range_reference &reference)
{
    return xlnt::range(*this, reference);
}

const range worksheet::range(const range_reference &reference) const
{
    return xlnt::range(*this, reference);
}

void worksheet::merge_cells(const std::string &reference_string)
{
    merge_cells(range_reference(reference_string));
}

void worksheet::unmerge_cells(const std::string &reference_string)
{
    unmerge_cells(range_reference(reference_string));
}

void worksheet::merge_cells(const range_reference &reference)
{
    d_->merged_cells_.push_back(reference);
    bool first = true;

    for (auto row : range(reference))
    {
        for (auto cell : row)
        {
            cell.merged(true);

            if (!first)
            {
                if (cell.data_type() == cell::type::shared_string)
                {
                    cell.value("");
                }
                else
                {
                    cell.clear_value();
                }
            }

            first = false;
        }
    }
}

void worksheet::unmerge_cells(const range_reference &reference)
{
    auto match = std::find(d_->merged_cells_.begin(), d_->merged_cells_.end(), reference);

    if (match == d_->merged_cells_.end())
    {
        throw invalid_parameter();
    }

    d_->merged_cells_.erase(match);

    for (auto row : range(reference))
    {
        for (auto cell : row)
        {
            cell.merged(false);
        }
    }
}

row_t worksheet::next_row() const
{
    auto row = highest_row() + 1;

    if (row == 2 && d_->cell_map_.size() == 0)
    {
        row = 1;
    }

    return row;
}

xlnt::range worksheet::rows(bool skip_null)
{
    return xlnt::range(*this, calculate_dimension(), major_order::row, skip_null);
}

const xlnt::range worksheet::rows(bool skip_null) const
{
    return xlnt::range(*this, calculate_dimension(), major_order::row, skip_null);
}

xlnt::range worksheet::columns(bool skip_null)
{
    return xlnt::range(*this, calculate_dimension(), major_order::column, skip_null);
}

const xlnt::range worksheet::columns(bool skip_null) const
{
    return xlnt::range(*this, calculate_dimension(), major_order::column, skip_null);
}

/*
//TODO: finish implementing cell_iterator wrapping before uncommenting

cell_vector worksheet::cells(bool skip_null)
{
    const auto dimension = calculate_dimension();
    return cell_vector(*this, dimension.top_left(), dimension, major_order::row, skip_null, true);
}

const cell_vector worksheet::cells(bool skip_null) const
{
    const auto dimension = calculate_dimension();
    return cell_vector(*this, dimension.top_left(), dimension, major_order::row, skip_null, true);
}
*/

void worksheet::clear_cell(const cell_reference &ref)
{
    d_->cell_map_.at(ref.row()).erase(ref.column());
    // TODO: garbage collect newly unreferenced resources such as styles?
}

void worksheet::clear_row(row_t row)
{
    d_->cell_map_.erase(row);
    // TODO: garbage collect newly unreferenced resources such as styles?
}

bool worksheet::operator==(const worksheet &other) const
{
    return compare(other, true);
}

bool worksheet::compare(const worksheet &other, bool reference) const
{
    if (reference)
    {
        return d_ == other.d_;
    }

    if (d_->parent_ != other.d_->parent_) return false;

    for (auto &row : d_->cell_map_)
    {
        if (other.d_->cell_map_.find(row.first) == other.d_->cell_map_.end())
        {
            return false;
        }

        for (auto &cell : row.second)
        {
            if (other.d_->cell_map_[row.first].find(cell.first) == other.d_->cell_map_[row.first].end())
            {
                return false;
            }

            xlnt::cell this_cell(&cell.second);
            xlnt::cell other_cell(&other.d_->cell_map_[row.first][cell.first]);

            if (this_cell.data_type() != other_cell.data_type())
            {
                return false;
            }

            if (this_cell.data_type() == xlnt::cell::type::number
                && std::fabs(this_cell.value<double>() - other_cell.value<double>()) > 0.0)
            {
                return false;
            }
        }
    }

    // todo: missing some comparisons

    if (d_->auto_filter_ == other.d_->auto_filter_ && d_->views_ == other.d_->views_
        && d_->merged_cells_ == other.d_->merged_cells_)
    {
        return true;
    }

    return false;
}

bool worksheet::operator!=(const worksheet &other) const
{
    return !(*this == other);
}

bool worksheet::operator==(std::nullptr_t) const
{
    return d_ == nullptr;
}

bool worksheet::operator!=(std::nullptr_t) const
{
    return d_ != nullptr;
}

void worksheet::operator=(const worksheet &other)
{
    d_ = other.d_;
}

const cell worksheet::operator[](const cell_reference &ref) const
{
    return cell(ref);
}

bool worksheet::has_named_range(const std::string &name) const
{
    return d_->named_ranges_.find(name) != d_->named_ranges_.end();
}

void worksheet::remove_named_range(const std::string &name)
{
    if (!has_named_range(name))
    {
        throw key_not_found();
    }

    d_->named_ranges_.erase(name);
}

void worksheet::reserve(std::size_t n)
{
    d_->cell_map_.reserve(n);
}

class header_footer worksheet::header_footer() const
{
    return d_->header_footer_.get();
}

cell_reference worksheet::point_pos(int left, int top) const
{
    column_t current_column = 1;
    row_t current_row = 1;

    double left_pos = 0;
    double top_pos = 0;

    while (left_pos <= left)
    {
        left_pos += column_width(current_column++);
    }

    while (top_pos <= top)
    {
        top_pos += row_height(current_row++);
    }

    return {current_column - 1, current_row - 1};
}

void worksheet::sheet_state(xlnt::sheet_state state)
{
    page_setup().sheet_state(state);
}

sheet_state worksheet::sheet_state() const
{
    return page_setup().sheet_state();
}

void worksheet::add_column_properties(column_t column, const xlnt::column_properties &props)
{
    d_->column_properties_[column] = props;
}

bool worksheet::has_column_properties(column_t column) const
{
    return d_->column_properties_.find(column) != d_->column_properties_.end();
}

column_properties &worksheet::column_properties(column_t column)
{
    return d_->column_properties_[column];
}

const column_properties &worksheet::column_properties(column_t column) const
{
    return d_->column_properties_.at(column);
}

row_properties &worksheet::row_properties(row_t row)
{
    return d_->row_properties_[row];
}

const row_properties &worksheet::row_properties(row_t row) const
{
    return d_->row_properties_.at(row);
}

void worksheet::add_row_properties(row_t row, const xlnt::row_properties &props)
{
    d_->row_properties_[row] = props;
}

worksheet::iterator worksheet::begin()
{
    return rows().begin();
}

worksheet::iterator worksheet::end()
{
    return rows().end();
}

worksheet::const_iterator worksheet::cbegin() const
{
    return rows().cbegin();
}

worksheet::const_iterator worksheet::cend() const
{
    return rows().cend();
}

worksheet::const_iterator worksheet::begin() const
{
    return cbegin();
}

worksheet::const_iterator worksheet::end() const
{
    return cend();
}

void worksheet::print_title_rows(row_t last_row)
{
    print_title_rows(1, last_row);
}

void worksheet::print_title_rows(row_t first_row, row_t last_row)
{
    d_->print_title_rows_ = std::to_string(first_row) + ":" + std::to_string(last_row);
}

void worksheet::print_title_cols(column_t last_column)
{
    print_title_cols(1, last_column);
}

void worksheet::print_title_cols(column_t first_column, column_t last_column)
{
    d_->print_title_cols_ = first_column.column_string() + ":" + last_column.column_string();
}

std::string worksheet::print_titles() const
{
    if (!d_->print_title_rows_.empty() && !d_->print_title_cols_.empty())
    {
        return d_->title_ + "!" + d_->print_title_rows_ + "," + d_->title_ + "!" + d_->print_title_cols_;
    }
    else if (!d_->print_title_cols_.empty())
    {
        return d_->title_ + "!" + d_->print_title_cols_;
    }
    else
    {
        return d_->title_ + "!" + d_->print_title_rows_;
    }
}

void worksheet::print_area(const std::string &print_area)
{
    d_->print_area_ = range_reference::make_absolute(range_reference(print_area));
}

range_reference worksheet::print_area() const
{
    return d_->print_area_.get();
}

bool worksheet::has_view() const
{
    return !d_->views_.empty();
}

sheet_view worksheet::view(std::size_t index) const
{
    return d_->views_.at(index);
}

void worksheet::add_view(const sheet_view &new_view)
{
    d_->views_.push_back(new_view);
}

void worksheet::register_comments_in_manifest()
{
    workbook().register_worksheet_part(*this, relationship_type::comments);
}

void worksheet::register_calc_chain_in_manifest()
{
    workbook().register_workbook_part(relationship_type::calculation_chain);
}

bool worksheet::has_header_footer() const
{
    return d_->header_footer_.is_set();
}

void worksheet::header_footer(const class header_footer &hf)
{
    d_->header_footer_ = hf;
}

void worksheet::clear_page_breaks()
{
    d_->row_breaks_.clear();
    d_->column_breaks_.clear();
}

void worksheet::page_break_at_row(row_t row)
{
    d_->row_breaks_.push_back(row);
}

const std::vector<row_t> &worksheet::page_break_rows() const
{
    return d_->row_breaks_;
}

void worksheet::page_break_at_column(xlnt::column_t column)
{
    d_->column_breaks_.push_back(column);
}

const std::vector<column_t> &worksheet::page_break_columns() const
{
    return d_->column_breaks_;
}

double worksheet::column_width(column_t column) const
{
    static const auto DefaultColumnWidth = 51.85;

    if (has_column_properties(column))
    {
        return column_properties(column).width.get();
    }
    else
    {
        return points_to_pixels(DefaultColumnWidth, 96.0);
    }
}

double worksheet::row_height(row_t row) const
{
    static const auto DefaultRowHeight = 15.0;

    if (has_row_properties(row))
    {
        return row_properties(row).height.get();
    }
    else
    {
        return points_to_pixels(DefaultRowHeight, 96.0);
    }
}

void worksheet::garbage_collect_formulae()
{
    workbook().garbage_collect_formulae();
}

void worksheet::parent(xlnt::workbook &wb)
{
    d_->parent_ = &wb;
}

conditional_format worksheet::conditional_format(const range_reference &ref, const condition &when)
{
	return workbook().d_->stylesheet_.get().add_conditional_format_rule(d_, ref, when);
}

} // namespace xlnt
