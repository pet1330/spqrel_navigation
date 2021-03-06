#pragma once
#include "srrg_types/types.hpp"



namespace spqrel_navigation {

  using namespace srrg_core;

  class DynamicMap {
    
    struct Vector2iComparator {
      bool operator () (const Eigen::Vector2i& v1, const Eigen::Vector2i& v2){
	if (v1.x() < v2.x() || (v1.x() == v2.x() && v1.y() < v2.y()))
	  return true;
	return false;
      }
    };
    
    //Map indexed by cell to have unique cells. A unique index is assigned to go back and forth from grid to map pose
    typedef std::map<Eigen::Vector2i, int, Vector2iComparator, Eigen::aligned_allocator< std::pair<Eigen::Vector2i,int> > > CellIndexMap;
    typedef std::map<int, Eigen::Vector2f, std::less<int>, Eigen::aligned_allocator< std::pair<int,Eigen::Vector2f> > > PointIndexMap;
    
  public:
    DynamicMap();

    inline void setMapResolution(const float map_resolution) {_map_resolution = map_resolution; _map_inverse_resolution = 1./_map_resolution;}
    inline void setFOV(const float fov){_fov = fov;}
    inline void setNumRanges(const int num_ranges){_num_ranges = num_ranges;}
    inline void setDistanceThreshold(const float distance_threshold){_distance_threshold = distance_threshold;}
    inline void setRobotPose(const Eigen::Isometry2f& robot_pose) {_robot_pose = robot_pose;}
    void setCurrentPoints(const Vector2fVector& current_points);

    inline void addBlindZone(const float from, const float to) {
      _blind_zones.push_back(Eigen::Vector2f(from, to));
    }
    
    void compute();
    void getOccupiedCells(Vector2iVector& occupied_cells);
    
    inline void clearPoints(){ _occupied_cells.clear();}
    
  protected:
    // Set of unique occupied cells
    CellIndexMap _occupied_cells; 
    Eigen::Isometry2f _robot_pose;
    PointIndexMap _current_points;
    
    float _map_resolution;
    float _map_inverse_resolution;
    float _fov; //240 degrees for pepper
    int _num_ranges; // 60 for pepper
    float _angle_increment;
    float _distance_threshold;
    Vector2fVector _blind_zones;
    
    inline Eigen::Vector2i world2grid(const Eigen::Vector2f p) {
      return Eigen::Vector2i(p.x()*_map_inverse_resolution, p.y()*_map_inverse_resolution);
    }
    
    inline Eigen::Vector2f grid2world(const Eigen::Vector2i p) {
      return Eigen::Vector2f(p.x()*_map_resolution, p.y()*_map_resolution);
    }

    //void addPoints(const Vector2iVector& points);

    
    bool findCellByIndex(const CellIndexMap& cellIndexMap, const int index, Eigen::Vector2i& cell);
    void transformPointsToRobot(PointIndexMap& transformedPoints);
    void transformPointsToMap(const PointIndexMap& points);
    void projectPoints(FloatVector& ranges, IntVector& indices, const PointIndexMap& points);
    void applyBlindZones(IntVector& indices);
    void merge(PointIndexMap& points_merged,
	       const FloatVector& ranges_previous, const IntVector& indices_previous,
	       const PointIndexMap& points_previous,
	       const FloatVector& ranges_current, const IntVector& indices_current,
	       const PointIndexMap& points_current);
    void reindex();

  };
  
}





