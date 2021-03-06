#pragma once

#include "public.h"
#include "keydata.h"
#include "declarations.h"
#include "Object.h"
#include "Visible.h"
#include "Cursor.h"
#include "Item.h"
#include "NPC.h"
#include "GUIElemContainer.h"
#include "GUIRowContainer.h"
#include "Meter.h"
#include "ParticleSystem.h"
#include "Inventory.h"
#include "Button.h"

class GameVariables : public Object
{
public:
    map<String,String> string_vars;
    map<String,int> int_vars;

    bool IfDef(String name)
    {
        if(string_vars.count(name))
            return true;

        if(int_vars.count(name))
            return true;

        return false;
    }

    void Increment(String name)
    {
        if(!int_vars.count(name))
            int_vars[name]=1;
        else
            int_vars[name]++;
    }
    int GetInt(String name)
    {
        if(int_vars.count(name))
            return int_vars[name];

        return -1;
    }
    String GetString(String name)
    {
        if(string_vars.count(name))
            return string_vars[name];

        return "error";
    }
    void SetInt(String name, int value)
    {
        int_vars[name] = value;
    }
    void SetString(String name, String value)
    {
        string_vars[name] = value;
    }
    void Define(String name)
    {
        SetString(name,"true");
    }
    void Undef(String name)
    {
        string_vars.erase(name);
    }
    void Print()
    {
        cout << "String variables: " << string_vars.size() << endl;
        for(auto i : string_vars)
            cout << i.first << " : " << i.second << endl;

        cout << "Int variables: " << int_vars.size() << endl;
        for(auto i : int_vars)
            cout << i.first << " : " << i.second << endl;
    }
};


static Entity Door(String portal, int x, int y,int wherex= 0, int wherey = 0)
{
    Entity ent;
    ent = Entity(images[ids["door"]],x,y,TILESIZE,TILESIZE);
    ent.alt = 1*TILESIZE;
    ent.string_attribs["portal"] = portal;
    ent.string_attribs["where"] = to_string(wherex*TILESIZE)+" "+to_string(wherey*TILESIZE);

    return ent;
}
static bool OnScreen(Entity * ent, float cX, float cY)
{
    if(ent->x - ent->offset/2 > cX + SCREEN_WIDTH || ent->x + ent->w + ent->offset/2 < cX
        || ent->y - ent->alt > cY + SCREEN_HEIGHT || ent->y + ent->h < cY)
            return false;

    return true;
}
static bool Intersect(Entity * ent1, Entity* ent2)
{
    if(ent1->x+1 > ent2->x + ent2->w-1 || ent1->x + ent1->w-1 < ent2->x+1
    || ent1->y+1 > ent2->y + ent2->h-1 || ent1->y + ent1->h-1 < ent2->y+1)
        return false;

    return true;
}
static bool Intersect(Entity * ent, int x, int y, int w, int h)
{
    if(ent->x+1 > x+w-1 || ent->x + ent->w < x+1
    || ent->y+1 > y+h-1 || ent->y + ent->h-1 < y+1)
        return false;

    return true;
}


static bool Sorter(Entity* ent1, Entity* ent2)
{
    if(!(ent1))
        return false;

    if(!(ent2))
        return true;

    if((ent1)->y == (ent2)->y)
    {
        if((ent1)->x == (ent2)->x)
            return (ent1)->id < (ent2)->id;
        else
            return (ent1)->x < (ent2)->x;

    }

    return (ent1)->y < (ent2)->y;
}


class CraftingRecipe
{
public:
    String item;
    int count;
    vector<String> ingredients;
    vector<int> counts;
    String medium;
    String unlock;
    String skill;
    int level;
    bool unlocked;

};
class Player : public Creature
{
public:
    NPC * pet = NULL;
    Meter * progressBar;

    int lastdir;
    bool moved;
    float current_frame = 0;

    Player()
    {
        lastdir = 0;
    }
    Player(Creature c) : Creature(c)
    {
        lastdir = 0;
        offset = TILESIZE/2;
        alt = TILESIZE/2;
        string_attribs["activity"]="none";
        //progressBar = new Meter(x,y,w,5,&x,&x,"green","dark","horizontal");

    }
    int Update()
    {
        Creature::Update();

        if(string_attribs["activity"]!="none")
        {
            int_attribs["activitytimer"]--;
            if(int_attribs["activitytimer"]<=0)
                string_attribs["activity"]="none";
        }
        if(pet)
            pet->AI();
    }
    void setPet(NPC * pet)
    {
        this->pet = pet;
        pet->setPlayer(this);
    }
    void Heal(int amount)
    {
        if(stat_levels["health"]+amount > stat_levels["maxhealth"])
            stat_levels["health"]=stat_levels["maxhealth"];
        else
            stat_levels["health"]+=amount;


        if(this->stat_levels["health"] <= 0)
            dead=true;
    }
    void Damage(int damage)
    {
        cout << "Player is attacked!" << endl;
        this->stat_levels["health"] -= damage;

        if(this->stat_levels["health"] <= 0)
            dead=true;
        /*cout << "Int attributes: " << endl;
        for(auto i : this->int_attribs)
            cout << i.first << " : " << to_string(i.second) << endl;

        cout << "String attributes: " << endl;
        for(auto i : this->string_attribs)
            cout << i.first << " : " << to_string(i.second) << endl;*/
    }
    void Draw(float cX = 0, float cY = 0)
    {
        if(string_attribs["activity"]!="none")
        {
            DrawStringCentered(string_attribs["activity"],x+w/2-cX,y-alt-cY-FONTSIZE);
        }
        if(!dead)
            DrawClip(img,x-cX-offset/2,y-cY-alt,w+offset,h+alt,floor(current_frame)*50,lastdir*(64),50,64,g("player-anim-frames")*50,4*(64));
        else
            DrawImage(im("gravestone"),x-cX,y-cY,TILESIZE,TILESIZE);


    }
};


class Map : public Object
{
public:
    int tile[100][100];
    int w,h;

    Map()
    {
        for(int i = 0; i < 100; i++)
        {
            for(int j = 0; j < 100; j++)
            {
                tile[i][j]=0;
            }
        }

        //tile[5][6]=1;
        w = 100;
        h = 100;

    }
    Map(int z)
    {
        w = h = z;
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
                tile[i][j]=1;
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        int tX = cX/TILESIZE;
        int tY = cY/TILESIZE;

        for(int i = tX-1; i < tX + SCREEN_WIDTH/TILESIZE+2; i++)
        {
            for(int j = tY-1; j < tY + SCREEN_HEIGHT/TILESIZE+2; j++)
            {
                if(i >= 0 && i < w && j >= 0 && j < h)
                {
                    DrawClip(images[ids["tileset"]],i*TILESIZE-cX,j*TILESIZE-cY,TILESIZE,TILESIZE,(tile[i][j]%g("tiles-horizontal"))*TILESIZE,(tile[i][j]/g("tiles-horizontal"))*TILESIZE,TILESIZE,TILESIZE,g("tiles-horizontal")*TILESIZE,(int)ceil((double)g("tiles")/g("tiles-horizontal"))*TILESIZE);
                }
            }
        }
    }
};

class WeatherSystem : public Object
{
public:
    String weather = "rain";
    String climate = "mediterranean";
    ParticleSystem * ps = NULL;
    bool active = true;
    const static int particles = 100;
    float particleSpeed = 8;
    float coordinates[particles][3];
    float limits[particles];

    WeatherSystem()
    {

    }
    WeatherSystem(ParticleSystem * _ps)
    {
        ps = _ps;
        fr(0,particles)
        {
            coordinates[i][1]= -1 * rand()%(SCREEN_HEIGHT);
            coordinates[i][0] = rand()%((int)(SCREEN_WIDTH*1.5));
            coordinates[i][2]= -coordinates[i][1]/SCREEN_HEIGHT;
            limits[i] = 0;//coordinates[i][1]+SCREEN_HEIGHT;
        }

    }

    void FeedClimate(String climate)
    {
        this->climate = climate;
    }
    int Update()
    {
        if(active && weather!="none" && climate=="continental")
        fr(0,particles)
        {
            particleSpeed = 8*coordinates[i][2]+6;
            coordinates[i][1] += particleSpeed*cos(3.14/6);
            coordinates[i][0] -= particleSpeed*sin(3.14/6);
            if(coordinates[i][1] > limits[i] || coordinates[i][0] < 0)
            {
                ps->Add(coordinates[i][0]+camera_x,coordinates[i][1]+camera_y,globals["particle-size"],globals["particle-size"],images[ids["water"]],1);

                coordinates[i][1]= -1 * rand()%(SCREEN_HEIGHT);
                coordinates[i][0] = rand()%((int)(SCREEN_WIDTH*1.5));
                coordinates[i][2]=-coordinates[i][1]/SCREEN_HEIGHT;
                limits[i] = coordinates[i][1]+SCREEN_HEIGHT;
            }
        }
        return 1;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        if(active && weather != "none" && climate=="continental")
        fr(0,particles)
        {
            DrawImage((weather=="rain"?images[ids[particle]]:images[ids["snow"]]),coordinates[i][0]+camera_xvel,coordinates[i][1]+camera_yvel,TILESIZE,TILESIZE);
        }
    }
};


class LightingSystem : public Object
{
public:
    float lightmap[400][400];
    int w = SCREEN_WIDTH/(LIGHTSIZE)+1;
    int h = SCREEN_HEIGHT/(LIGHTSIZE)+1;
    float globallight;

    LightingSystem()
    {
        globallight = 0.8;
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
            {
                lightmap[i][j]= (i*1.0/w+j*1.0/h) / 2;
            }
        }
    }
    void Draw()
    {
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < w; j++)
            {
                /*if(lightmap[i][j] == 1.0)
                    printf("wutifak\n");*/

                DrawRect(i*LIGHTSIZE,j*LIGHTSIZE,LIGHTSIZE,LIGHTSIZE,lightmap[i][j]);
            }
        }
        DrawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,globallight);
    }

    int Update()
    {

        globallight = (current_hour*100-900+current_minute)*1.00/(2359-900);
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
            {
                lightmap[i][j]= globallight;//(i*1.0/w+j*1.0/h) / 2;
            }
        }

        for(float i = 0; i < 20; i++)
        {
            for(float j = 0; j < 20; j++)
            {
                if(sqrt(i*i+j*j) < 20)
                {

                    ///TODO CLEANUP !!!!
                    int currentx = KeyData.MouseX/LIGHTSIZE - i;
                    int currenty = KeyData.MouseY/LIGHTSIZE - j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0.0+sqrt(i*i+j*j)/20*globallight;
                    }

                    currentx = KeyData.MouseX/LIGHTSIZE + i;
                    currenty = KeyData.MouseY/LIGHTSIZE - j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }
                    currentx = KeyData.MouseX/LIGHTSIZE - i;
                    currenty = KeyData.MouseY/LIGHTSIZE + j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }

                    currentx = KeyData.MouseX/LIGHTSIZE + i;
                    currenty = KeyData.MouseY/LIGHTSIZE + j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }
                }
            }
        }


        return 1;
    }
};

static map<String, NPC> npc_templates;
static map<String, Item> item_templates;
static map<String, Entity> object_templates;
static vector<String> object_names;
static map<String, CraftingRecipe> recipes;

static bool LoadObjects()
{
    printf("Loading objects from XML list...\n");
    using namespace tinyxml2;

    XMLDocument xmlDoc;
    XMLError result;

    result = xmlDoc.LoadFile("data/xml/object.xml");

    XMLCheckResult(result);
    printf("XML file loaded..\n");

    XMLElement * pRoot = xmlDoc.FirstChildElement();
    if(pRoot == NULL)
    {
        printf("XML read error (root)\n");
        return 0;
    }
    XMLElement * pElement = pRoot->FirstChildElement();
    if(pElement == NULL)
    {
        printf("XML read error!\n");
        return 0;
    }
    int id;

    //object_templates.push_back(Entity());
    object_templates.clear();
    object_names.clear();

    while (pElement != NULL)
    {
        printf("Loading object\n");

        //pElement->QueryIntAttribute("id", &id);

        Entity newobject;

        newobject.name = pElement->FirstChildElement("name")->GetText();

        object_names.push_back(newobject.name);

        newobject.img = images[ids[pElement->FirstChildElement("img")->GetText()]];
        //newobject.max_stack = atoi(pElement->FirstChildElement("stack")->GetText());
        newobject.category = pElement->FirstChildElement("category")->GetText();
        newobject.type = pElement->FirstChildElement("type")->GetText();
        newobject.w = (pElement->FirstChildElement("width")?atoi(pElement->FirstChildElement("width")->GetText())*TILESIZE:TILESIZE);
        newobject.h = (pElement->FirstChildElement("height")?atoi(pElement->FirstChildElement("height")->GetText())*TILESIZE:TILESIZE);

        if(pElement->FirstChildElement("rigid") != NULL)
            newobject.rigid = true;

        if(pElement->FirstChildElement("pick") != NULL)
        {
            newobject.pickable = true;
            newobject.pick = pElement->FirstChildElement("pick")->GetText();
        }
        if(pElement->FirstChildElement("leftover") != NULL)
        {
            newobject.leftover = pElement->FirstChildElement("leftover")->GetText();
        }
        if(pElement->FirstChildElement("growth")!= NULL)
        {
            newobject.int_attribs["growth"] = 0;
            newobject.int_attribs["cycle"] = atoi(pElement->FirstChildElement("growth")->GetText());
            newobject.string_attribs["ripe"] = pElement->FirstChildElement("ripe")->GetText();
        }
        else
        {
            //newobject.int_attribs["growth"] = 0;
            //newobject.int_attribs["cycle"] = 1;

        }

        if(pElement->FirstChildElement("destroy") != NULL)
            newobject.destroy = pElement->FirstChildElement("destroy")->GetText();

        if(pElement->FirstChildElement("hits") != NULL)
            newobject.hits = atoi(pElement->FirstChildElement("hits")->GetText());
        else
            newobject.hits = 1;

        newobject.health = newobject.hits;

        if(pElement->FirstChildElement("alt_img") != NULL)
            newobject.alts["damaged"] = images[ids[pElement->FirstChildElement("alt_img")->GetText()]];

        if(pElement->FirstChildElement("fade") != NULL)
            newobject.alts["fade"] = images[ids[pElement->FirstChildElement("fade")->GetText()]];

        if(pElement->FirstChildElement("damaged_fade") != NULL)
            newobject.alts["damaged_fade"] = images[ids[pElement->FirstChildElement("damaged_fade")->GetText()]];

        if(pElement->FirstChildElement("alt") != NULL)
        {
            newobject.alt = atoi(pElement->FirstChildElement("alt")->GetText())*TILESIZE;
        }

        if(pElement->FirstChildElement("offset") != NULL)
        {
            newobject.offset = atoi(pElement->FirstChildElement("offset")->GetText())*TILESIZE;
        }

        if(pElement->FirstChildElement("imgw") != NULL)
            newobject.imgw = atoi(pElement->FirstChildElement("imgw")->GetText())*TILESIZE;
        else
            newobject.imgw = TILESIZE;

        if(pElement->FirstChildElement("imgh") != NULL)
            newobject.imgh = atoi(pElement->FirstChildElement("imgh")->GetText())*TILESIZE;
        else
            newobject.imgh = TILESIZE;

        if(pElement->FirstChildElement("clipw") != NULL)
            newobject.clipw = atoi(pElement->FirstChildElement("clipw")->GetText())*TILESIZE;
        else
            newobject.clipw = newobject.imgw;

        if(pElement->FirstChildElement("cliph") != NULL)
            newobject.cliph = atoi(pElement->FirstChildElement("cliph")->GetText())*TILESIZE;
        else
            newobject.cliph = newobject.imgh;

        if(pElement->FirstChildElement("portal")!= NULL)
        {
            newobject.string_attribs["portal"] = pElement->FirstChildElement("portal")->GetText();
        }
        if(pElement->FirstChildElement("attributes")!=NULL)
        {
            XMLElement * listElement = pElement->FirstChildElement("attributes")->FirstChildElement();

            while(listElement)
            {
                if(listElement->GetText())
                {
                    if(is_number(listElement->GetText()))
                    {
                        newobject.int_attribs[listElement->Name()] = atoi(listElement->GetText());

                    }
                    else
                    {
                        newobject.string_attribs[listElement->Name()] = listElement->GetText();

                    }
                }
                else
                    newobject.int_attribs[listElement->Name()] = 1;
                listElement = listElement->NextSiblingElement();
            }
        }
        if(pElement->FirstChildElement("drops")!=NULL)
        {
            XMLElement * listElement = pElement->FirstChildElement("drops")->FirstChildElement();

            while(listElement)
            {
                newobject.drops.push_back(listElement->FirstChildElement("item")->GetText());
                newobject.mins.push_back(atoi(listElement->FirstChildElement("min")->GetText()));
                newobject.maxs.push_back(atoi(listElement->FirstChildElement("max")->GetText()));
                listElement = listElement->NextSiblingElement();
            }
        }
        if(pElement->FirstChildElement("interactions"))
        {
            XMLElement* interaction = pElement->FirstChildElement("interactions")->FirstChildElement("i");

            while(interaction)
            {
                Entity::Interaction intr;
                intr.obj = interaction->FirstChildElement("obj")->GetText();
                intr.script = interaction->FirstChildElement("script")->GetText();
                newobject.interactions.push_back(intr);
                interaction = interaction->NextSiblingElement();
            }
        }
        object_templates[newobject.name]=newobject;

        //object_ids[newobject.name]=id;

        pElement = pElement->NextSiblingElement("object");
    };

    printf("Finished loading objects!\n");
    return 1;
}

class PickupObject : public Entity
{
public:
    PickupObject(int _x, int _y, int _w, int _h, String item) : Entity(item_templates[item].img,_x,_y,_w,_h)
    {
        name = "nothing";

        pickable = true;
        pick = item;
    }

};
class Preview : public Entity
{
    public:
    bool active = false;
    Entity target;

    Preview()
    {
        name = "preview";
        active = false;
    }
    void Add(Entity tg)
    {
        target = tg;
        w = target.w;
        h = target.h;
        alt = target.alt;
        offset = target.offset;

        active = true;
    }
    void Disable()
    {
        active = false;
    }
    void Move(int mx, int my)
    {
        x = mx;
        y = my;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        if(active)
        {
            int place_x = std::floor((x-target.offset/2+cX)/TILESIZE);
            int place_y = std::floor((y-target.alt+cY)/TILESIZE);

            DrawClip(target.img,place_x*TILESIZE-cX,place_y*TILESIZE-cY,w+target.offset,h+alt,0,0,target.clipw,target.cliph,target.imgw,target.imgh,0.5);
        }
    }
};

class Stage : public Object
{
public:
    bool indoors = false;
    map<String,String> attributes;

    int w, h;
    String name;
    Map tilemap;
    Stage * exits[100];

    Entity * grid[MAP_MAX_SIZE][MAP_MAX_SIZE];

    vector<Entity *> objects;
    //vector<Entity *> ptr_to_npcs;

    vector<NPC *> npcs;

    enum {
        GRASS,
        WOOD,
        TILLED,
    };
    void Resize(int w, int h)
    {
        this->w = w;
        this->h = h;
        if(tilemap.w != w)
            tilemap.w = w;
        if(tilemap.h != h)
            tilemap.h = h;
    }
    void ClearGrid()
    {
        for(int i = 0; i < w; i++)
            for(int j = 0; j < h; j++)
                grid[i][j]=NULL;

    }
    void ScanGrid()
    {

        for(auto& o : objects)
        if(o)
            grid[(int)o->x/TILESIZE][(int)o->y/TILESIZE]=o;

    }


    Stage()
    {
        w = h = 100;

        ClearGrid();
    }
    Stage(int z)
    {
        tilemap = Map(z);
        w = h = z;
        ClearGrid();


    }
    bool IsFree(int x, int y)
    {
        return grid[x][y]==NULL;
    }
    bool IsSolid(int x, int y)
    {
        return (tilemap.tile[x][y] >= 38 && tilemap.tile[x][y] < 46) || tilemap.tile[x][y] == 16 || (tilemap.tile[x][y] >= 4 && tilemap.tile[x][y] <= 12);
    }
    bool CanPlant(int x, int y)
    {
        return tilemap.tile[x][y] == TILLED && IsFree(x,y);
    }

    bool IsGround(int x, int y)
    {
        return tilemap.tile[x][y] == GRASS;
    }

    void Set(int x, int y, int n)
    {
        if(n > g("tiles"))
            n = 0;

        tilemap.tile[x][y] = n;
    }
    int Get(int x, int y)
    {
        return tilemap.tile[x][y];
    }
    void Remove(Entity * en)
    {
        int x = std::floor(en->x/TILESIZE);
        int y = std::floor(en->y/TILESIZE);

        for(int i = 0; i < en->w/TILESIZE; i++)
            for(int j = 0; j < en->h/TILESIZE; j++)
                if(grid[x+i][y+j]==en)
                    grid[x+i][y+j]=0;
    }
    void Add(Entity  * en)
    {
        int x = std::floor(en->x/TILESIZE);
        int y = std::floor(en->y/TILESIZE);

        for(int i = 0; i < en->w/TILESIZE; i++)
        {
            for(int j = 0; j < en->h/TILESIZE; j++)
            {
                if(x+i > 0 && x+i < w && y+j > 0 && y+j < h)
                    grid[x+i][y+j] = en;
            }
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        tilemap.Draw(cX,cY);
    }
};

class XMLManager
{
public:
    static Stage * LoadStage(String path)
    {
        Stage * stage = new Stage();

        using namespace tinyxml2;

        XMLDocument doc;
        doc.LoadFile(path.c_str());

        XMLElement * root = doc.FirstChildElement();

        stage->name = root->FirstChildElement("name")->GetText();
        String dimensions = root->FirstChildElement("size")->GetText();
        stage->w = atoi(FirstWord(dimensions).c_str());
        stage->h = atoi(SecondWord(dimensions).c_str());

        if(root->FirstChildElement("indoors"))
            stage->indoors = true;

        if(root->FirstChildElement("attributes"))
        {
            XMLElement * attribute = root->FirstChildElement("attributes")->FirstChildElement();

            while(attribute)
            {
                stage->attributes[attribute->Name()] = attribute->GetText();

                attribute = attribute->NextSiblingElement();
            }

        }

        XMLElement * rowElement = root->FirstChildElement("row");
        int height = 0;
        while(rowElement)
        {
            std::stringstream ss;

            String row = rowElement->GetText();

            ss << row;

            fr(0,stage->w)
                {
                    String current;
                    ss >> current;
                    stage->tilemap.tile[i][height] = atoi(current);
                }

            rowElement = rowElement->NextSiblingElement("row");
            height++;
        }

        XMLElement * objectElement = root->FirstChildElement("object");

        while(objectElement)
        {
            Entity * new_object;

            String name = objectElement->FirstChildElement("name")->GetText();
            String position = objectElement->FirstChildElement("pos")->GetText();

            if(name != "door")
            {
                new_object = new Entity(object_templates[name],atoi(FirstWord(position).c_str()),atoi(SecondWord(position)));

            }
            else
            {
                new_object = new Entity(Door("lot",atoi(FirstWord(position).c_str()),atoi(SecondWord(position))));

            }


            XMLElement* attributeElement = NULL;
            if(objectElement->FirstChildElement("attributes") != NULL)
                attributeElement = objectElement->FirstChildElement("attributes")->FirstChildElement();

            while(attributeElement)
            {
                String name = attributeElement->Name();
                String value = attributeElement->GetText();

                if(is_number(value))
                    new_object->int_attribs[name] = atoi(value);
                else
                    new_object->string_attribs[name] = value;

                attributeElement = attributeElement->NextSiblingElement();
            }


            stage->Add(new_object);
            stage->objects.push_back(new_object);

            objectElement = objectElement->NextSiblingElement("object");


        }

        XMLElement * npcElement = root->FirstChildElement("npc");

        while(npcElement)
        {
            NPC * new_object;

            String name = npcElement->FirstChildElement("name")->GetText();
            String position = npcElement->FirstChildElement("pos")->GetText();

            new_object = new NPC(npc_templates[name],atoi(FirstWord(position)),atoi(SecondWord(position).c_str()));

            new_object->name = name;

            if(npcElement->FirstChildElement("met"))
                new_object->met = true;

            stage->npcs.push_back(new_object);


            npcElement = npcElement->NextSiblingElement("npc");
        }

        return stage;
    }
    static void SaveStage(Stage * st, String path)
    {
        using namespace tinyxml2;

        XMLDocument doc;

        XMLNode * root = doc.NewElement("root");

        doc.InsertFirstChild(root);

        XMLElement * pElement;

        auto InsertInt = [&pElement, &root, &doc](String name, int value, XMLNode* elem)
        {
            pElement = doc.NewElement(name.c_str());
            pElement->SetText(value);
            elem->InsertEndChild(pElement);

        };
        auto InsertString = [&pElement, &root, &doc](String name, String value, XMLNode* elem )
        {
            pElement = doc.NewElement(name.c_str());
            pElement->SetText(value.c_str());
            elem->InsertEndChild(pElement);

        };

        InsertString("name",st->name,root);
        InsertString("size",to_string(st->w)+" "+to_string(st->h),root);

        if(st->indoors)
            root->InsertEndChild(doc.NewElement("indoors"));

        fr(0,st->h)
        {
            String row = "";
            for(int j = 0; j < st->w; j++)
                row.append(to_string(st->tilemap.tile[j][i])+" ");

            InsertString("row",row,root);
        }
        for(auto e : st->objects)
        if(e)
        {
            XMLElement * obj = doc.NewElement("object");

            if(e->name != "")
                InsertString("name",e->name,obj);
            else
                InsertString("name","door",obj);

            InsertString("pos",to_string(e->x)+" "+to_string(e->y),obj);

            XMLElement * attribs = doc.NewElement("attributes");

            for(auto a : e->int_attribs)
            {
                InsertString(a.first,to_string(a.second),attribs);
            }
            for(auto a : e->string_attribs)
            {
                InsertString(a.first,a.second,attribs);
            }
            if(attribs->FirstChildElement())
                obj->InsertEndChild(attribs);

            root->InsertEndChild(obj);


        }
        for(auto e : st->npcs)
        if(e)
        {
            XMLElement * obj = doc.NewElement("npc");

            InsertString("name",e->name,obj);

            InsertString("pos",to_string(e->x)+" "+to_string(e->y),obj);

            XMLElement * attribs = doc.NewElement("attributes");

            for(auto a : e->int_attribs)
            {
                InsertString(a.first,to_string(a.second),attribs);
            }
            for(auto a : e->string_attribs)
            {
                InsertString(a.first,a.second,attribs);
            }
            if(attribs->FirstChildElement())
                obj->InsertEndChild(attribs);

            root->InsertEndChild(obj);


        }
        doc.SaveFile(path.c_str());
    }
};

class Task
{
public:
    bool complete = false;
    String text;
    String name;
    vector<String> strings;
    vector<int> ints;

    Task()
    {}
    Task( String t,  String n, vector<String> ss = {}, vector<int> is = {}) : text(t), name(n), strings(ss), ints(is)
    {

    }
};
static vector<Task> main_tasks{Task("Cut down a tree","DESTROY",{"tree"}),Task("Talk to the shopkeeper","TALK",{"shopkeeper"}),Task("Plant 10 crops","PLACE",{"plant"})};
static vector<Task> daily_tasks{Task("Eat an apple","EAT",{"apple"})};

class EventSystem : public Object
{
public:
    Task mainTask;
    Task dailyTask;

    EventSystem()
    {
        mainTask = main_tasks[main_progress];
        int randnumber = rand()%tasks.size();
        dailyTask = Task(tasks[randnumber],events[randnumber]);

    }
    void Event(String name, vector<String> strings = {}, vector<int> ints = {})
    {
        //cout << strings[0] << endl;
        if(!mainTask.complete && name == mainTask.name)
        {
            bool complete = true;
            for(int i = 0; i < std::min(mainTask.strings.size(),strings.size()); i++)
            {
                //printf("checking");
                //cout << mainTask.strings[i] << endl;

                if(!(mainTask.strings[i] == strings[i] || mainTask.strings[i]=="NEVERMIND"))
                    complete = false;
            }

            if(complete)
            {
                alerts.push("Main task complete!");
                main_progress++;
                if(main_progress < main_tasks.size())
                    mainTask = main_tasks[main_progress];
                else
                    mainTask.complete = true;

            }
        }

        if(!dailyTask.complete && name == dailyTask.name)
        {
            dailyTask.complete = true;
            alerts.push("Daily task complete!");

        }
    }
};

class HealthBar : public Entity
{
    public:
    NPC * n = NULL;
    bool active = false;
    int d = 5;
    int maxfade = 15;
    int fade = 15;
    HealthBar()
    {
        y = 0;
        h = d+FONTSIZE+d+TILESIZE/2+d;
        w = SCREEN_WIDTH/4;
        x = SCREEN_WIDTH/2-w/2;
    }
    void Add(NPC *n)
    {
        this->n = n;
        active = true;
        fade = maxfade;
    }

    int Update()
    {
        if(n && n->int_attribs["hp"]<=0)
        {
            n->int_attribs["hp"] = 0;
            if(fade > 1)
                fade--;
            else
                active = false;

        }
    }

    void Draw(float cX = 0, float cY = 0)
    {
        if(active && n)
        {
            float alph = (float)fade/maxfade;
            DrawImage(im("gui"),x,y,w,h, alph);
            DrawString(n->fullname,x+w/2-lineWidth(n->fullname)/2,d);
            DrawImage(im("gui2"),x+d,y+d+FONTSIZE+d,w-2*d,TILESIZE/2,alph);
            DrawImage(im("red"),x+d,y+d+FONTSIZE+d,n->int_attribs["hp"]*(w-2*d)/n->int_attribs["health"],TILESIZE/2,alph);
        }
        else
        {
            active = false;
        }
    }
};
class HUD : public Entity
{
    Meter * healthBar;

public:
    String displaystring;
    String timestring;
    String taskstring = "Main Task:";
    String dailystring = "Daily Task:";

    EventSystem * eventsys;
    Player * p;

    HUD(EventSystem * sys, Player * pl)
    {
        displaystring = "Year " + to_string(current_year) + ", " + seasons[current_season-1] + ", Day " + to_string(current_day);

        timestring = "Time:" + (current_hour < 10 ? String(" 0") : String(" ")) + to_string(current_hour) + (current_minute < 10 ? String(":0") : String(":")) + to_string(current_minute);

        w = displaystring.size()*FONTSIZE;
        h = FONTSIZE;
        y = 0;
        x = SCREEN_WIDTH-lineWidth(displaystring);

        eventsys = sys;
        p = pl;

        const int healthBarLength = 4*TILESIZE;
        const int healthBarHeight = TILESIZE/2;
        const int healthBarDistance = TILESIZE/2;
        healthBar = new Meter(SCREEN_WIDTH-healthBarHeight-healthBarDistance,
                              SCREEN_HEIGHT-healthBarLength,
                              healthBarHeight,
                              healthBarLength,
                              &p->stat_levels["health"],
                              &p->stat_levels["maxhealth"],
                              "red",
                              "dark",
                              "vertical");

    }

    int Update()
    {
        displaystring = "Year " + to_string(current_year) + ", " + seasons[current_season-1] + ", Day " + to_string(current_day);

        timestring = "Time:" + (current_hour < 10 ? String(" 0") : String(" ")) + to_string(current_hour) + (current_minute < 10 ? String(":0") : String(":")) + to_string(current_minute);

        w = displaystring.size()*FONTSIZE;
        h = FONTSIZE;
        y = 0;
        x = SCREEN_WIDTH-lineWidth(displaystring);

        return 1;
    }
    void DrawLine(String s, int &cY)
    {
        DrawImage(im("fade"),SCREEN_WIDTH-lineWidth(s),cY,lineWidth(s),FONTSIZE);
        DrawString(s,SCREEN_WIDTH-lineWidth(s),cY,-1,"font2");
        cY += FONTSIZE;
    }
    void DrawHealthBar()
    {

    }
    void Draw(float cX = 0, float cY = 0)
    {
        int pY = 0;

        DrawLine(displaystring,pY);
        //DrawImage(images[ids["fade"]],x,y,w,h);
        //DrawString(displaystring,x,y,-1,"font2");

        DrawLine(timestring,pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,w,h);
        //DrawString(timestring,SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,-1,"font2");

        if(p)
        DrawLine("Health: " + to_string(p->stat_levels["health"]),pY);

        DrawLine("Hunger: " + hunger_levels[(int)floor(hunger/HUNGER_SCALE)],pY);

        DrawLine(taskstring,pY);
        if(!eventsys->mainTask.complete)
        {
            DrawLine(eventsys->mainTask.text,pY);
        }
        else
        {

        }

        DrawLine(dailystring,pY);
        if(!eventsys->dailyTask.complete)
        {
            DrawLine(eventsys->dailyTask.text,pY);
        }
        else
        {

        }
        if(g("debug"))
        {
            String coordinates = to_string(camera_x+KeyData.MouseX)+ " " + to_string(camera_y+KeyData.MouseY);
            DrawImage(im("fade"),0,0,lineWidth(coordinates),FONTSIZE);
            DrawString(coordinates,0,0,-1,"font2");

        }


        healthBar->Draw();
    }
};
class Dialog
{
public:
    image portrait;

    String speaker;
    vector<String> chain;
    bool hasQuestion = false;

    String question;
    vector<String> answers;
    vector<String> outcomes;
    vector<int> gotos;
    vector<String> conditions;

    Dialog(){};
    Dialog(String msg){
        speaker="narrator";
        question=msg;
        answers.push_back("OK");
        outcomes.push_back("");
        gotos.push_back(0);
        conditions.push_back("");
    };

};

static Dialog Prompt(String msg, String script, String positive = "Yes", String negative = "No")
{
    Dialog d;
    d.speaker = "narrator";
    d.question=msg;
    d.answers.push_back(positive);
    d.answers.push_back(negative);
    d.outcomes.push_back(script);
    d.outcomes.push_back("");
    d.gotos.push_back(0);
    d.gotos.push_back(0);
    d.conditions.push_back("");
    d.conditions.push_back("");
    return d;
}

class DialogSystem : public Dialog, public Entity
{
    GameVariables * gameVars;

public:
    int base_h;
    //int x,y,w,h;
    int portrait_size;
    int textLength, currentLength;
    int lineLength, ptSize;


    int borderw = 11;
    Dialog Current;
    int currentstring;
    String displaystring;
    int stringsize;
    int selected_answer = 0;
    bool finished_scroll = false;
    bool finished_chain = false;
    int namePtSize = FONTSIZE;
    int lines = 0;

    void setGameVariables(GameVariables * gameVars)
    {
        this->gameVars = gameVars;
    }
    void calculateLines(String c)
    {
        std::stringstream ss;
        vector<String> words;
        ss << c;
        String newword;
        while(ss >> newword){words.push_back(newword);};
        int i = 0;
        int current_length = 0;
        lines = 1;
        while(i < words.size())
        {
            if(current_length + lineWidth(words[i] + " ",ptSize) + 1 > lineLength)
            {
                lines++;
                current_length = 0;
            }
            else
            {
                current_length += lineWidth(words[i] + " ",ptSize);
                i++;
            }
        }
    }
    bool ProcessCondition(String s)
    {
        if(s == "")
            return true;

        if(FirstWord(s) == "skill")
        {
            if(skills[SecondWord(s)]>=atoi(nWord(3,s)))
                return true;
        }
        else if(FirstWord(s) == "ifdef")
        {
            if(gameVars)
                if(gameVars->IfDef(SecondWord(s)))
                    return true;
        }
        else if(FirstWord(s) == "ifndef")
        {
            if(gameVars)
                if( ! gameVars->IfDef(SecondWord(s)))
                    return true;
        }
        else if(FirstWord(s) == "ge")
        {
            if(gameVars)
                if( gameVars->GetInt(SecondWord(s)) >= atoi(nWord(3,s).c_str()))
                    return true;
        }
        return false;
    }
    void Add(Dialog _dialog, String default_sp = "narrator")
    {
        if(_dialog.speaker != "any")
            speaker = _dialog.speaker;
        else
            speaker = default_sp;

        chain = _dialog.chain;
        hasQuestion = _dialog.hasQuestion;
        question = _dialog.question;
        answers.clear();gotos.clear();outcomes.clear();

        if(npc_templates[speaker].attributes.count("sound"))
        {
            audioCommands.push("sound " + npc_templates[speaker].attributes["sound"]);
            cout << "adding sound\n";

        }
        for(int i = 0; i < _dialog.answers.size(); i++)
        if(ProcessCondition(_dialog.conditions[i]))
        {
            answers.push_back(_dialog.answers[i]);
            gotos.push_back(_dialog.gotos[i]);
            outcomes.push_back(_dialog.outcomes[i]);
        }
        //answers = _dialog.answers;
        //gotos = _dialog.gotos;
        //outcomes = _dialog.outcomes;

        if(chain.size()>0)
        {
            displaystring = chain[0];
            currentstring = 0;
            stringsize = 0;
            finished_scroll = false;
            finished_chain = false;
            selected_answer = 0;

        }
        else
        {
            displaystring = question;
            currentstring = 0;
            stringsize = 0;
            finished_chain = true;
            finished_scroll = false;
            selected_answer = 0;
        }

        namePtSize = FONTSIZE;
        while(lineWidth(npc_templates[speaker].fullname,namePtSize) > base_h-5)
        {
            namePtSize--;
        }

    }
    void Scroll()
    {
        if(!finished_chain)
        {
            if(stringsize < chain[currentstring].size())
            {
                stringsize++;
            }
            if(stringsize == chain[currentstring].size())
                finished_scroll = true;

        }
        else
        {
            if(stringsize < question.size())
            {
                stringsize++;
            }
            if(stringsize == question.size())
                finished_scroll = true;
        }
    }
    void Next()
    {
        currentstring++;
        if(currentstring < chain.size())
        {
            finished_scroll = false;
            displaystring = chain[currentstring];
            stringsize = 0;
        }
        else
        {
            finished_chain = true;
            displaystring = question;
            stringsize = 0;
            finished_scroll = false;
        }
    }
    void Draw()
    {
        if(finished_chain)
        {
            h = base_h + (answers.size()-1)*ptSize;
        }
        else
            h = base_h;

        y = SCREEN_HEIGHT-h;
        DrawImage(images[ids["dialog"]],x,y,w,h);

        if(speaker != "narrator")
        {
            DrawImage(images[ids["gui"]],x-base_h,y,base_h,base_h);
            DrawImage(portrait,x-base_h+borderw,y+borderw,base_h-borderw*2,base_h-borderw*2);
            DrawImage(images[ids["portraitframe"]],x-base_h,y,base_h,base_h);
            DrawString(npc_templates[speaker].fullname,x-base_h,y+base_h-namePtSize,namePtSize);
        }
        String drawstring;
        for(int i = 0; i < stringsize; i++)
            drawstring.push_back(displaystring[i]);

        DrawWrapped(drawstring,x+15,y+10, lineLength, ptSize);

        if(finished_chain && finished_scroll)
        {
            calculateLines(drawstring);

            DrawLetter('>',x+15-ptSize,y+15+lines*ptSize+selected_answer*(ptSize+5),ptSize);

            for(int i = 0; i < answers.size(); i++)
                DrawString(answers[i],x+15,y+15+lines*ptSize+i*(ptSize+5),ptSize);
        }
    }

    void Hover(int mx, int my)
    {
        if(finished_chain)
        if(my > y+15+lines*ptSize)
        {
            my -= y+15+lines*ptSize;
            selected_answer = (int)floor(my/(ptSize+5));
            if(selected_answer > answers.size()-1)
                selected_answer = answers.size()-1;
        }
    }
    DialogSystem()
    {
        w = globals["dialog-width"];
        h = base_h = globals["dialog-height"];
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT-h;
        lineLength = globals["dialog-line-length"];
        ptSize = FONTSIZE;//16;
    };

};



class NumberSelector : public GUI
{
public:
    int number=0,max=99;
    int def=0;
    NumberSelector()
    {

    }
    NumberSelector(int _x, int _y,int _max=99, int _default=1)
    {
        number=def=_default;
        max=_max;
        w = g("number-selector-w");
        h = g("number-selector-h");
        x = _x; y = _y;
    }
    void Reset()
    {
        number=def;
    }
    int Change(int amount)
    {
        if(number+amount>0)
            number += amount;
        else if(number+amount<=0)
            number = max-amount-1;

        if(number>max)
            number-=max;
    }
    int HandleClick(int mX, int mY)
    {
        mX -= x; mY -= y;
        if(mX > w-TILESIZE)
        {
            Change((mY>h/2?-1:1));
        }
    }
    void Draw(float cX=0, float cY=0)
    {
        DrawImage(im("gui2"),x,y,w,h);

        DrawImage(im("itemtile"),x+w-TILESIZE,y,TILESIZE,h/2);
        DrawStringCentered("+",x+w-TILESIZE/2,y,10);
        DrawImage(im("itemtile"),x+w-TILESIZE,y+h/2,TILESIZE,h/2);
        DrawStringCentered("-",x+w-TILESIZE/2,y+h/2,10);

        DrawString(to_string(number),x,y+h/2-FONTSIZE/2);

    }
};



class GUITabContainer : public GUI
{
public:
    vector<GUI*> tabs;
    int activeTab = 0;
    int toolbarHeight = TILESIZE;


    void Scroll(int dir)
    {
        tabs[activeTab]->Scroll(dir);
    }
    void AddElem(GUI * elem)
    {
        tabs.push_back(elem);
        w = elem->w;
        h = elem->h + toolbarHeight;
        elem->SetPosition(SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2+toolbarHeight);
    }
    GUITabContainer()
    {

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        for(int i = 0; i < tabs.size(); i++)
        {
            DrawImage(im("itemtile"),x+i*toolbarHeight,y,toolbarHeight,toolbarHeight);
        }
        if(tabs.size() > activeTab)
        {
            tabs[activeTab]->Draw();
        }
    }

    void SwitchTab(int i)
    {
        activeTab = i;
        w = tabs[i]->w;
        h = tabs[i]->h + toolbarHeight;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
    }
    int HandleClick(int mX, int mY)
    {
        if(mY-y < toolbarHeight)
        {
            if(mX-x < toolbarHeight * tabs.size())
            {
                SwitchTab(floor((mX-x)/toolbarHeight));
            }
        }
        else
        {
            tabs[activeTab]->HandleClick(mX,mY);

        }
    }
    void Exit()
    {
        for(auto g : tabs)
        {
            g->Exit();
        }
    }
};

class GUIInventoryItems : public GUI
{
  public:
        Inventory * inv;
        int frame = TILESIZE/2;
        bool holdingItem = false;
        Item * hold;

        GUIInventoryItems()
        {

        }
        GUIInventoryItems(Inventory * _inv)
        {
            type = "inventory";
            inv = _inv;
            w = 10*TILESIZE + 2*frame;
            h = 4*TILESIZE + 2*frame + TILESIZE + frame;
            x = SCREEN_WIDTH/2 - w/2;
            y = SCREEN_HEIGHT/2 - h/2;

        }
        void Draw(float cX = 0, float cY = 0)
        {
            DrawImage(images[ids["gui"]],x,y,w,h);
            //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 10; j++)
                {

                    DrawImage(images[ids["itemtile"]],x+frame+j*TILESIZE,y+frame+i*TILESIZE,TILESIZE,TILESIZE);
                    if(inv->items[i*10+j])
                    {
                        DrawImage(inv->items[i*10+j]->img,x+frame+j*TILESIZE,y+frame+i*TILESIZE,TILESIZE,TILESIZE);
                        if(inv->items[i*10+j]->quantity > 1)
                            DrawString(to_string(inv->items[i*10+j]->quantity),x+frame+j*TILESIZE+TILESIZE-(inv->items[i*10+j]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+frame+i*TILESIZE+TILESIZE-FONTSIZE,-1,"font2");
                    }
                    if(i*10+j>=inv->allowedSize)
                        DrawImage(im("cross"),x+frame+j*TILESIZE,y+frame+i*TILESIZE,TILESIZE,TILESIZE);


                }

            for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
            {
                DrawImage(images[ids["itemtile"]],x+frame+i*TILESIZE,y+h-frame-TILESIZE,TILESIZE,TILESIZE);
                if(inv->toolbar[i])
                {
                    DrawImage(inv->toolbar[i]->img,x+frame+i*TILESIZE,y+h-frame-TILESIZE,TILESIZE,TILESIZE);
                    if(inv->toolbar[i]->quantity > 1)
                        DrawString(to_string(inv->toolbar[i]->quantity),x+frame+i*TILESIZE+TILESIZE-(inv->toolbar[i]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+h-frame-FONTSIZE,-1,"font2");

                }
            }

            if(holdingItem)
                if(hold)
                {
                    DrawImage(hold->img,KeyData.MouseX,KeyData.MouseY,TILESIZE,TILESIZE);
                    if(hold->quantity > 1)
                        DrawString(to_string(hold->quantity),KeyData.MouseX+TILESIZE-(hold->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,KeyData.MouseY+TILESIZE-FONTSIZE,-1,"font2");

                }


        }

        void Exit()
        {
            if(holdingItem)
            if(hold)
            {
                int q = hold->quantity;
                hold->quantity = 1;
                inv->Add(*hold,q);
                hold = NULL;
            }
        }
        Item* GetClick(int mX, int mY)
        {
            if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
            {
                int item_i = (mX-x-frame)/TILESIZE;
                int item_j = (mY-y-frame)/TILESIZE;
                if(inv->items[item_j*10+item_i])
                {
                    return inv->items[item_j*10+item_i];


                }

            }
            if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
            {
                int item_i = (mX-x-frame)/TILESIZE;
                if(inv->toolbar[item_i])
                {
                    return inv->toolbar[item_i];
                }

            }

            return NULL;
        }
        int HandleClick(int mX, int mY)
        {
            if(!holdingItem)
            {
                if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
                {

                    int item_i = (mX-x-frame)/TILESIZE;
                    int item_j = (mY-y-frame)/TILESIZE;
                    int item_index = item_j*10+item_i;

                    if(item_index < inv->allowedSize && inv->items[item_index])
                    {
                        hold = inv->items[item_index];
                        holdingItem = true;
                        inv->items[item_index] = NULL;


                    }
                }
                if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
                {
                    //TOOLBAR
                    int item_i = (mX-x-frame)/TILESIZE;
                    if(inv->toolbar[item_i])
                    {
                        hold = inv->toolbar[item_i];
                        holdingItem = true;
                        inv->toolbar[item_i] = NULL;


                    }
                }
            }
            else
            {
                if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    int item_j = (mY-y-frame)/TILESIZE;
                    int item_index = item_j*10+item_i;
                    if(item_index < inv->allowedSize){

                        if(!inv->items[item_index])
                        {
                            inv->items[item_index] = hold;
                            hold = NULL;
                            holdingItem = false;
                        }
                        else
                        {
                            Item* temp = hold;
                            hold = inv->items[item_index];
                            inv->items[item_index] = temp;
                        }
                    }
                }
                if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    if(!inv->toolbar[item_i])
                    {
                        inv->toolbar[item_i] = hold;
                        hold = NULL;
                        holdingItem = false;


                    }
                    else
                    {
                        Item* temp = hold;
                        hold = inv->toolbar[item_i];
                        inv->toolbar[item_i] = temp;
                    }

                }
            }
        }
};

class GUIBuyItems : public GUI
{
public:

    vector<Item> item_list;
    Inventory * inv;
    int scrollPosition = 0;

    void Scroll(int dir)
    {
        if(item_list.size()>g("shop-shown-items"))
        {
            if(scrollPosition + dir >= 0 && scrollPosition+dir <= item_list.size()-g("shop-shown-items"))
                scrollPosition += dir;
        }
    }
    GUIBuyItems(Inventory * inv, String store_inventory = "general-store-inventory")
    {

        for(auto& e : allWord(s(store_inventory)))
            item_list.push_back(item_templates[e]);

        type = "shop";
        w = 400;
        h = std::min(g("shop-shown-items")*TILESIZE,(int)item_list.size()*TILESIZE+TILESIZE);
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
        this->inv = inv;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        int i = 0;
        for(int ii = scrollPosition; ii< scrollPosition+std::min(g("shop-shown-items"),(int)item_list.size()); ii++)
        {
            Item e = item_list[ii];
            i = ii-scrollPosition;
            DrawSeparator(x,y+TILESIZE+i*TILESIZE,w,TILESIZE);
            DrawImage(e.img,x,y+TILESIZE+i*TILESIZE,TILESIZE,TILESIZE);
            DrawString(e.formal,x+TILESIZE,y+TILESIZE+i*TILESIZE);
            String pricestring = to_string(e.int_attribs["price"])+"" + CURRENCY;
            DrawString(pricestring,x+w-lineWidth(pricestring)-FONTSIZE,y+TILESIZE+i*TILESIZE);

        }

        if(item_list.size()>g("shop-shown-items"))
        {
            DrawImage(im("itemtile"),x+w-5,y,5,h);
            int nodeh = g("shop-shown-items")/item_list.size()*h;

            DrawImage(im("button"),x+w-5,y+(float)scrollPosition*(nodeh)/item_list.size(),5,5);
        }
    }
    int HandleClick(int mX, int mY)
    {
        int clickY = mY - y - TILESIZE;
        if(clickY < 0)
            return NULL;
        clickY /= TILESIZE;
        clickY += scrollPosition;

        if(clickY < item_list.size())
        {
            if(item_list[clickY].int_attribs["price"] <= funds)
            {
                funds -= item_list[clickY].int_attribs["price"];
                inv->Add(item_list[clickY]);
                return 1;
            }
        }

        return 0;
    }
    Item *GetClick(int mX, int mY, int& id)
    {
        int clickY = mY - y - TILESIZE;
        if(clickY < 0)
            return NULL;
        clickY /= TILESIZE;

        if(clickY < item_list.size())
            return &item_list[clickY];

        return NULL;
    }
};

class GUIBuyScreen : public GUIRowContainer
{
public:
    GUIBuyItems * buyItems;
    GUIInventoryItems * inventoryItems;

    GUIBuyScreen(Inventory * inv, String shop_inventory)
    {
        buyItems = new GUIBuyItems(inv,shop_inventory);
        inventoryItems = new GUIInventoryItems(inv);

        AddElem(buyItems);
        AddElem(inventoryItems);
    }

    void Sell(Item * item)
    {
        if(item->int_attribs.count("sell"))
            funds += item->int_attribs["sell"]*item->quantity;
        inventoryItems->inv->Remove(item->name,item->quantity);
    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(inventoryItems,mX,mY,0,0))
        {
            Item * clickItem = inventoryItems->GetClick(mX,mY);
            if(clickItem)
                Sell(clickItem);
        }
        else if(Contains(buyItems,mX,mY,0,0))
        {
            buyItems->HandleClick(mX,mY);
        }
    }
};
class GUISellItems : public GUI
{
public:
    Inventory * inv;
    GUISellItems(){};
    GUISellItems(Inventory * _inv)
    {
        type = "sell";
        inv = _inv;
        w = INVENTORY_TOOLBAR_SIZE*TILESIZE;
        h = 2*TILESIZE;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        DrawString("funds: " + to_string(funds) + " " + CURRENCY,x,y);
        //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        {
            DrawImage(images[ids["itemtile"]],x+i*TILESIZE,y+h-TILESIZE,TILESIZE,TILESIZE);
            if(inv->toolbar[i])
            {
                DrawImage(inv->toolbar[i]->img,x+i*TILESIZE,y+h-TILESIZE,TILESIZE,TILESIZE);
                if(inv->toolbar[i]->quantity > 1)
                    DrawString(to_string(inv->toolbar[i]->quantity),x+i*TILESIZE+TILESIZE-(inv->toolbar[i]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+h-FONTSIZE,-1,"font2");
            }
        }
    }
    Item *GetClick(int mX, int mY, int& id)
    {
        int clickX = mX - x;
        int clickY = mY - y;
        clickX /= TILESIZE;

        if(clickY > TILESIZE)
        {
            id = clickX;
            return inv->toolbar[clickX];

        }
        return NULL;
    }
};

class GUISellScreen : public GUIRowContainer
{
  public:

    GUISellItems * sellItems;
    GUIInventoryItems * items;

    bool holdingItem = false;
    Item selectedItem;

    GUISellScreen(Inventory * inv)
    {
        sellItems = new GUISellItems(inv);
        items = new GUIInventoryItems(inv);

        AddElem(sellItems);
        AddElem(items);
    }

    void SwapWithSelected(int id)
    {
    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(items,mX,mY,0,0))
        {
            int id;
            if(items->GetClick(mX,mY))
            {
            }
        }
        else
        {

        }
    }
};
class GUIElemSkills : public GUI
{
    public:
    vector<String> skill_names = {"Skills"};
    vector<String> skill_levels = {""};
    int maxwidth = 0;

    GUIElemSkills(int _x, int _y)
    {
        w = 2*globals["gui-border"]+globals["gui-width"]*3/4;
        h = 2*globals["gui-border"]+FONTSIZE;

        for(auto skill : skills)
        {
            h += FONTSIZE + globals["gui-margin"];
            skill_names.push_back(skill.first);
            skill_levels.push_back(to_string(skill.second));
            if(lineWidth(skill.first + ":") > maxwidth)
                maxwidth = lineWidth(skill.first + ":");

        }
        x = _x;
        y = _y;

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        int pY = y+globals["gui-border"];//y+h/2-skill_names.size()*(FONTSIZE+globals["gui-margin"]);
        for(int i = 0; i < skill_names.size(); i++)
        {
            DrawString(skill_names[i]+":",x+globals["gui-margin"],pY);
            if(i > 0)
            {

                int maximumw = x+w-globals["gui-margin"]-FONTSIZE-globals["gui-margin"]-x-maxwidth-globals["gui-margin"];
                DrawImage(images[ids["fade"]],x+maxwidth+globals["gui-margin"],pY,maximumw,FONTSIZE);
                DrawImage(images[ids["dark"]],x+maxwidth+globals["gui-margin"],pY,xps[skill_names[i]]*maximumw/levelcap(skills[skill_names[i]]),FONTSIZE);
                DrawString(skill_levels[i],x+w-globals["gui-margin"]-FONTSIZE,pY);

            }
            pY += FONTSIZE+globals["gui-margin"];
        }
    }
};

class GUIShopScreen : public GUITabContainer
{
public:

    GUIShopScreen(Inventory * inv, String shop_inventory) : GUITabContainer()
    {
        type = "shop";

        AddElem(new GUIBuyScreen(inv, shop_inventory));
        AddElem(new GUISellScreen(inv));

        SwitchTab(0);
    }
};

class GUIToolSelect : public GUI
{

public:
    String * tool;

    GUIToolSelect(String * tool)
    {
        x = y = 0;
        w = 2*TILESIZE;
        h = 2*TILESIZE;
        this->tool = tool;
    }
    int HandleClick(int mX, int mY)
    {
        mX -= x;
        mY -= y;
        if(mY < TILESIZE)
        {
            if(mX < TILESIZE)
                *tool = "terraform";
            else
                *tool = "object";
        }
        else
        {
            if(mX < TILESIZE)
                *tool = "delete";
        }

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawImage(im("itemtile"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("hoe"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("itemtile"),x+TILESIZE,y,TILESIZE,TILESIZE);
        DrawImage(im("chair"),x+TILESIZE,y,TILESIZE,TILESIZE);
        DrawImage(im("itemtile"),x,y+TILESIZE,TILESIZE,TILESIZE);
        DrawImage(im("staff-red"),x,y+TILESIZE,TILESIZE,TILESIZE);
    }
};

class GUISaveLoadEditor : public GUI
{
public:
    String * commands;

    GUISaveLoadEditor(String * commands)
    {
        this->commands = commands;
        x=y=0;
        w = 2 * TILESIZE;
        h = TILESIZE;

    }
    int HandleClick(int mX, int mY)
    {
        mX -= x;
        mY -= y;
        if(mX < TILESIZE)
        {
            *commands = "save";
        }
        else
        {
            *commands = "load";
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawImage(im("coffee"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("chest"),x+TILESIZE,y,TILESIZE,TILESIZE);
    }

};
class GUIEditorInterface : public GUIRowContainer
{
public:
    NumberSelector * widthSetter,  *heightSetter;
    Stage * targetStage;


    GUIEditorInterface(Stage * targetStage, String * tool, String * commands)
    {
        x = y = w = h = 0;
        w = 0;

        this->targetStage = targetStage;

        widthSetter = new NumberSelector(0,0,100,10);
        heightSetter = new NumberSelector(0,0,100,10);

        AddElem(widthSetter);
        AddElem(heightSetter);
        AddElem(new GUIToolSelect(tool));
        AddElem(new GUISaveLoadEditor(commands));

        x = SCREEN_WIDTH-w;

        for(auto g : rows)
            g->x = SCREEN_WIDTH-w;

    }

    int Update()
    {
        targetStage->Resize(widthSetter->number,heightSetter->number);
    }
};
class GUIElemEquips : public GUI
{
    public:
    GUIElemEquips(int _x, int _y)
    {
        x = _x;
        y = _y;
        w = globals["gui-width"]/4;
        h = 2*globals["gui-border"]+FONTSIZE+skills.size()*(FONTSIZE + globals["gui-margin"]);
    }
    void DrawSlot(int &pY, String str)
    {
        DrawString(str,x+w/2-lineWidth(str)/2,pY);
        pY+=FONTSIZE;
        DrawImage(im("itemtile"),x+w/2-TILESIZE/2,pY,TILESIZE,TILESIZE);
        pY+=TILESIZE;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        int pY = y+g("gui-border");
        DrawSlot(pY,"Hat");
        DrawSlot(pY,"Ring");
        DrawSlot(pY,"Shoes");
    }
};

class GUIElemStats : public GUI
{
    public:
    vector<String> lines;
    vector<int> modifications;

    int spacing = 5;
    Player * pl;

    GUIElemStats(Player * player)
    {
        w = 20;
        h = 20;
        pl = player;
        lines = allWord(s("player-stats"));
        h = lines.size()*(FONTSIZE+spacing);
        x = 0;
        y = SCREEN_HEIGHT/2-h/2;
        for(auto &l : lines)
        {
            if(lineWidth(Capitalize(l))>w)
                w = lineWidth(Capitalize(l));

            int currentModification = 0;
            for(int i = 0; i < pl->modifiers.size(); i++)
            {
                if(pl->modifiers[i].stat == l)
                    currentModification += pl->modifiers[i].points;
            }
            modifications.push_back(currentModification);
        }
        w+=5 + TILESIZE*3;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);
        int pY = y+h/2-lines.size()*(FONTSIZE+spacing)/2.0;
        for(int i = 0; i < lines.size(); i++)
        {
            String l = lines[i];
            DrawString(Capitalize(l)+":",x,pY);
            String statNumberString = to_string(pl->stat_levels[l])+(modifications[i]>0?"+"+to_string(modifications[i]):"")+(modifications[i]<0?"-"+to_string(-modifications[i]):"");
            DrawString(statNumberString,x+w-lineWidth(statNumberString)-TILESIZE/2,pY);
            pY += FONTSIZE+spacing;
        }
    }
};
class GUICharacterScreen : public GUIRowContainer
{
    public:
    GUIElemSkills *skills;
    GUIElemEquips *equips;
    Player * pl;

    GUICharacterScreen(Player * player)
    {
        pl = player;
        w = 0;//2*globals["gui-border"]+1.5*globals["gui-width"];
        h = 2*globals["gui-border"]+FONTSIZE;
        skills = new GUIElemSkills(SCREEN_WIDTH/2-w/2+globals["gui-width"]/2,SCREEN_HEIGHT/2-h/2);
        equips = new GUIElemEquips(SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2);

        AddElem(equips);
        AddElem(skills);
        AddElem(new GUIElemStats(pl));

    }



};

class GUIItemDescription : public GUI
{
public:
    String focus = "";

    GUIItemDescription()
    {
        w = 300;
        h = 200;
    }

    void OnNewItemSelected(String new_item)
    {
        focus = new_item;


    }
    void DeselectItem()
    {
        focus = "";
    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawCentered(im("itemtile"),x+w/2,y+h/4,TILESIZE,TILESIZE);
        DrawCentered(im("gui2"),x+w/2,y+2*h/3,w*3/4,h/2);
        if(focus != "")
        {
            DrawCentered(item_templates[focus].img,x+w/2,y+h/4,TILESIZE,TILESIZE);
            DrawStringCentered(item_templates[focus].formal,x+w/2,y+h/4-TILESIZE);
            if(item_templates[focus].desc != "")
                DrawWrapped(item_templates[focus].desc,x+w/2-w*3/8,y+2*h/3-h/4,w*3/4);
        }
    }
};
class GUIInventoryScreen : public GUIElemContainer
{
    public:

    GUIInventoryItems *inv_items;
    GUIItemDescription *item_description;
    Inventory * inv;

    GUIInventoryScreen()
    {

    }
    GUIInventoryScreen(Inventory * inv)
    {
        w = 0;
        h = 0;
        type = "inventory";
        inv_items = new GUIInventoryItems(inv);
        item_description = new GUIItemDescription();
        this->inv = inv;

        AddElem(inv_items);
        AddElem(item_description);
    }

    int HandleMouseMove(int mX, int mY)
    {
        if(Contains(inv_items,mX,mY,0,0))
        {
            if(inv_items->GetClick(mX,mY))
            {
                item_description->OnNewItemSelected(inv_items->GetClick(mX,mY)->name);
            }
            else
                item_description->DeselectItem();
        }
    }

};
class GUIEscapeScreen : public GUI
{
public:
    GUIEscapeScreen()
    {

    }
    void Draw(float cX = 0, float cY = 0)
    {

    }
};
class GUICraftingScreen : public GUI
{
public:
    Inventory * inv;
    NumberSelector numSel;
    vector<CraftingRecipe> recipe_list;
    String medium;
    int selected = 0;
    bool can_craft = false;

    int bx,by,bw,bh;

    GUICraftingScreen()
    {

    }
    GUICraftingScreen(Inventory * _inv, String _medium)
    {
        type = "crafting";
        inv = _inv;
        medium = _medium;

        for(auto& e : recipes)
        {
            if(e.second.medium == medium)
                if(skills[e.second.skill] >= e.second.level)
                    if(item_templates.count(e.second.item))
                        recipe_list.push_back(e.second);
        }
        w = 500;
        h = 300;//recipe_list.size() * TILESIZE;
        x = SCREEN_WIDTH/2 - w/2;
        y = SCREEN_HEIGHT/2 - h/2;

        bw = 75;
        bh = 50;
        bx = x + w*3/4 - bw/2;
        by = y + h*3/4 - bh/2;

        numSel = NumberSelector(x+w*3/4-g("number-selector-w")/2,y+h/2);

        TestCraftability();
        RefreshSize();
    }
    void RefreshSize()
    {
        w = 500;
        h = recipe_list.size()*TILESIZE;
        numSel.y = y+h/2;
        by = numSel.y+numSel.h+5;
        const int DISTANCE_BETWEEN = 16;

        if(selected < recipe_list.size())
        {
            int slide = y + TILESIZE*3 - 5 + DISTANCE_BETWEEN + linesCount(item_templates[recipe_list[selected].item].desc, w/2-TILESIZE*2)*FONTSIZE - numSel.y;
            if(slide > 0)
            {
                //h+= slide;
                numSel.y += slide;
                by += slide;

            }


        }

    }
    void Scroll(int dir)
    {
        selected += dir;
        if(selected < 0)
            selected = recipe_list.size()-1;
        else if(selected >= recipe_list.size())
            selected = 0;

        RefreshSize();
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        for(int i = 0;  i < recipe_list.size(); i++)
        {
            DrawImage(images[ids["itemtile"]],x,y+i*TILESIZE,TILESIZE,TILESIZE);
            if(item_templates.count(recipe_list[i].item))
                DrawImage(item_templates[recipe_list[i].item].img,x,y+i*TILESIZE,TILESIZE,TILESIZE);
            if(i == selected)
                //DrawImage(images[ids["frame"]],x,y+i*TILESIZE,TILESIZE,TILESIZE);
                DrawSeparator(x,y+i*TILESIZE,w/2,TILESIZE);

            DrawString(item_templates[recipe_list[i].item].formal,x+TILESIZE,y+i*TILESIZE);
        }

        if(selected < recipe_list.size())
        {
            int width = (2 + recipe_list[selected].ingredients.size())*TILESIZE;

            DrawImage(images[ids["itemtile"]],x + w*3/4 - width/2,y+TILESIZE,TILESIZE,TILESIZE);
            if(item_templates.count(recipe_list[selected].item))
                DrawImage(item_templates[recipe_list[selected].item].img,x + w*3/4 -width/2,y+TILESIZE,TILESIZE,TILESIZE);
            if(recipe_list[selected].count > 1)
                DrawString(to_string(recipe_list[selected].count),x + w*3/4 - width/2 + TILESIZE - (recipe_list[selected].count > 9 ? 2 : 1) * DIGITWIDTH,y+TILESIZE+TILESIZE-FONTSIZE,-1,"font2");

            for(int i = 0 ; i < recipe_list[selected].ingredients.size(); i++)
            {
                DrawImage(images[ids["itemtile"]],x + w*3/4 - width/2 + (2+i)*TILESIZE,y+TILESIZE,TILESIZE,TILESIZE);
                if(item_templates.count(recipe_list[selected].ingredients[i]))
                    DrawImage(item_templates[recipe_list[selected].ingredients[i]].img,x + w*3/4 - width/2 + (2+i)*TILESIZE,y+TILESIZE,TILESIZE,TILESIZE);
                if(recipe_list[selected].counts[i] > 1)
                    DrawString(to_string(recipe_list[selected].counts[i]),x + w*3/4 - width/2 + (2+i)*TILESIZE + TILESIZE - (recipe_list[selected].counts[i] > 9 ? 2 : 1) * DIGITWIDTH,y+TILESIZE+TILESIZE-FONTSIZE,-1,"font2");

            }

            DrawImage(im("gui2"),x + w/2+TILESIZE,y+TILESIZE*3-5, w/2-TILESIZE*2,linesCount(item_templates[recipe_list[selected].item].desc,w/2-TILESIZE*2)*FONTSIZE);
            DrawWrapped(item_templates[recipe_list[selected].item].desc, x + w/2 + TILESIZE, y + TILESIZE*3 - 5, w/2-TILESIZE*2);

            numSel.Draw();
            //if(can_craft)
                DrawImage(im("create"),bx,by,bw,bh);
            if(!can_craft)
                DrawImage(im("fade"),bx,by,bw,bh,0.9);
        }
    }

    void TestCraftability()
    {
        can_craft = true;
        for(int i = 0; i < recipe_list[selected].ingredients.size(); i++)
        {
            if(inv->Count(recipe_list[selected].ingredients[i]) < numSel.number*recipe_list[selected].counts[i])
            {
                //cout << inv->Count(recipe_list[selected].ingredients[i]) << " " << recipe_list[selected].counts[i] << endl;
                can_craft = false;
            }
        }

    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(&numSel,mX,mY,0,0))
        {
            numSel.HandleClick(mX,mY);
            TestCraftability();

        }
        if(mX < x + w/2)
        {
            if((mY-y)/TILESIZE < recipe_list.size())
            {
                if(selected != (mY-y)/TILESIZE)
                {
                    selected = (mY-y)/TILESIZE;
                    numSel.Reset();
                    TestCraftability();
                    RefreshSize();
                }

            }

            return 0;
        }
        else if(mX >= bx && mX <= bx + bw && mY >= by && mY <= by + bh)
        {

            if(can_craft)
                if(item_templates.count(recipe_list[selected].item))
                {
                    for(int j = 0; j < numSel.number; j++)
                        for(int i = 0; i < recipe_list[selected].ingredients.size(); i++)
                            inv->Remove(recipe_list[selected].ingredients[i],recipe_list[selected].counts[i]);

                    inv->Add(item_templates[recipe_list[selected].item], numSel.number*recipe_list[selected].count);

                    xps[recipe_list[selected].skill] += numSel.number*CRAFTING_XP;
                    while(xps[recipe_list[selected].skill] >= levelcap(skills[recipe_list[selected].skill]))
                    {
                        xps[recipe_list[selected].skill] -= levelcap(skills[recipe_list[selected].skill]);
                        skills[recipe_list[selected].skill]++;
                        alerts.push("LEVELUP! " + recipe_list[selected].skill + " lvl " + to_string(skills[recipe_list[selected].skill]));

                    }

                    TestCraftability();

                    return 1;
                }


            return 0;
        }
    }
};
