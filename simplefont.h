// clang-format off
// origin: https://opengameart.org/users/domsson

const uint32_t simplefont_bitmap_width           = 128;
const uint32_t simplefont_bitmap_height          = 64;
const uint32_t simplefont_bitmap_glyphs_per_row  = 18;
const uint32_t simplefont_bitmap_glyphs_width    = 5;
const uint32_t simplefont_bitmap_glyphs_pad_x    = 1;
const uint32_t simplefont_bitmap_glyphs_pad_y    = 1;
const uint32_t simplefont_bitmap_glyphs_height   = 7;
//const uint32_t simplefont_bitmap_glyphs_rows     = 6;
char const *simplefont_bitmap[] = {
"                                                                                                                                ",
"          *     * *    * *     *    **      ***     *       *    *                                                ***     *     ",
"          *     * *    * *    ****  **  *  *  *     *      *      *    * * *    *                             *  *   *   **     ",
"          *           *****  * *       *   * *            *        *    ***     *                            *   *  **  * *     ",
"          *            * *    ***     *     *             *        *   *****  *****         *****           *    * * *    *     ",
"          *           *****    * *   *     * * *          *        *    ***     *      *                   *     **  *    *     ",
"                       * *   ****   *  **  *  *            *      *    * * *    *      *                  *      *   *    *     ",
"          *            * *     *       **   ** *            *    *                    *              *            ***   *****   ",
"                                                                                                                                ",
"                                                                                                                                ",
"  ***    ***     **   *****   ***   *****   ***    ***                                       ***    ***    ***   ****    ***    ",
" *   *  *   *   * *   *      *          *  *   *  *   *    *      *       *           *     *   *  *   *  *   *  *   *  *   *   ",
"     *      *  *  *   *      *         *   *   *  *   *                  *    *****    *    *   *  * ***  *   *  *   *  *       ",
"    *     **   *****  ****   ****     *     ***    ****                 *               *      *   * * *  *****  ****   *       ",
"   *        *     *       *  *   *   *     *   *      *           *      *    *****    *      *    * ***  *   *  *   *  *       ",
"  *     *   *     *       *  *   *   *     *   *      *    *      *       *           *            *      *   *  *   *  *   *   ",
" *****   ***      *   ****    ***    *      ***    ***           *                            *     ***   *   *  ****    ***    ",
"                                                                                                                                ",
"                                                                                                                                ",
" ****   *****  *****   ***   *   *  *****  *****  *   *  *      *   *  *   *   ***   ****    ***   ****    ****  *****  *   *   ",
" *   *  *      *      *   *  *   *    *        *  *   *  *      ** **  *   *  *   *  *   *  *   *  *   *  *        *    *   *   ",
" *   *  *      *      *      *   *    *        *  *  *   *      * * *  **  *  *   *  *   *  *   *  *   *  *        *    *   *   ",
" *   *  ****   ****   *      *****    *        *  ***    *      *   *  * * *  *   *  ****   *   *  ****    ***     *    *   *   ",
" *   *  *      *      *  **  *   *    *        *  *  *   *      *   *  *  **  *   *  *      * * *  *   *      *    *    *   *   ",
" *   *  *      *      *   *  *   *    *    *   *  *   *  *      *   *  *   *  *   *  *      *  *   *   *      *    *    *   *   ",
" ****   *****  *       ***   *   *  *****   ***   *   *  *****  *   *  *   *   ***   *       ** *  *   *  ****     *     ***    ",
"                                                                                                                                ",
"                                                                                                                                ",
" *   *  *   *  *   *  *   *  *****    **           **      *            *            *                 *           **           ",
" *   *  *   *  *   *  *   *      *    *    *        *     * *            *           *                 *          *             ",
" *   *  *   *   * *    * *      *     *     *       *                          ***   ****    ***    ****   ***   *****   ****   ",
" *   *  *   *    *      *      *      *      *      *                             *  *   *  *   *  *   *  *   *   *     *   *   ",
" *   *  * * *   * *     *     *       *       *     *                          ****  *   *  *      *   *  *****   *      ****   ",
"  * *   ** **  *   *    *    *        *        *    *                         *   *  *   *  *   *  *   *  *       *         *   ",
"   *    *   *  *   *    *    *****    **           **           *****          ****  ****    ***    ****   ****   *      ***    ",
"                                                                                                                                ",
"                                                                                                                                ",
" *        *        *  *      *                                                        *                                         ",
" *                    *      *                                                        *                                         ",
" ****   ***      ***  *  *   *      ** *   ****    ***   ****    ****  * **    ****  ****   *   *  *   *  *   *  *   *  *   *   ",
" *   *    *        *  ***    *      * * *  *   *  *   *  *   *  *   *  **  *  *       *     *   *  *   *  *   *   * *   *   *   ",
" *   *    *        *  *  *   *      * * *  *   *  *   *  ****    ****  *       ***    *     *   *  *   *  *   *    *     ****   ",
" *   *    *    *   *  *   *  *      *   *  *   *  *   *  *          *  *          *   *  *  *   *   * *   * * *   * *       *   ",
" *   *  *****   ***   *   *   ***   *   *  *   *   ***   *          *  *      ****     **    ****    *     * *   *   *   ***    ",
"                                                                                                                                ",
"                                                                                                                                ",
"           *     *     *                                                                                                        ",
"          *      *      *                                                                                                       ",
" *****    *      *      *     *  *                                                                                              ",
"    *    *       *       *   * **                                                                                               ",
"   *      *      *      *                                                                                                       ",
"  *       *      *      *                                                                                                       ",
" *****     *     *     *                                                                                                        ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
"                                                                                                                                ",
};
// clang-format on
