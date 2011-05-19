#ifndef FEATURELISTS_H_INCLUDED
#define FEATURELISTS_H_INCLUDED

struct FeatureData
{
  char lost;
  char lost_since;

  unsigned int x,y,z;
  unsigned int mem;
  unsigned int group;
};

struct PointCorrespondence
{
   unsigned int mapped_to;
   unsigned int patch_width,patch_height;
   unsigned int score;
};

struct FeatureList
{
  unsigned int max_features;
  unsigned int current_features;
  struct FeatureData * list;

  unsigned int reg_for_correspondance_1;
  struct PointCorrespondence * correspondance_1;

  unsigned int reg_for_correspondance_2;
  struct PointCorrespondence * correspondance_2;

  unsigned int reg_for_correspondance_3;
  struct PointCorrespondence * correspondance_3;
};

struct FeatureList * CreateFeatureList(unsigned int size , unsigned int def_patch_width,unsigned int def_patch_height);
int DestroyFeatureList(struct FeatureList * list);
int CopyFeatureList(struct FeatureList * source,struct FeatureList * target);
int ClearFeatureList(struct FeatureList * list);
int GetFeatureData(struct FeatureList * list, unsigned int point_num,unsigned int data_type);
int PrintFeatureListContents(struct FeatureList * list);
#endif // FEATURELISTS_H_INCLUDED
