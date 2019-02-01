#include <string>
#include <vector>
#include <stdint.h>

#include <cv.h>
#include <highgui.h>
#include <opencv2/highgui/highgui_c.h>

#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

struct rect_t
{
  int32_t x,y;
  int32_t width, height;

  explicit rect_t(int32_t p_x = 0, int32_t p_y = 0, int32_t p_width = 0, int32_t p_height = 0) : x(p_x), y(p_y), width(p_width), height(p_height) {}

  int32_t x_middle_top() { return x + width/2; }
  int32_t y_middle_top() { return y; }
  int32_t x_middle_bot() { return x + width/2; }
  int32_t y_middle_bot() { return y + height; }
};

struct node_t
{
  std::string value;
  int32_t layer;
  int32_t pos;
  bool marker;
  std::vector<node_t*> prev;
  rect_t rect;
  int family;

  explicit node_t(std::string p_value, int p_layer = -1) : value(p_value), layer(p_layer), pos(-1), marker(false) {}
};

class GraphDrawer
{
public:
  GraphDrawer();
  ~GraphDrawer() {}

  void draw(std::string file_name);

protected:
  std::vector<node_t*> roots_nodes;
  std::vector<node_t*> branchs_nodes;

  std::vector<std::vector<node_t*>> layer_nodes;

  IplImage* image;
  bool exist(std::string value);

  void setRect(int layer, int nb_layer, int nb_index, node_t* node);
  void link();

  void putLayer(int layer);
  bool updateOneMarker(int layer);
  bool testEnd();

  cv::Scalar ScalarHSV2BGR(uint8_t H, uint8_t S, uint8_t V);
};

#endif /* GRAPHDRAWER_H */
