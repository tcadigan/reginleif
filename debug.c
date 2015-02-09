#include "debug.h"

void print_h_list(struct h_list *item, FILE *output)
{
    print_h_list_internal(item, 0, 0, output);
}

void print_h_list_internal(struct h_list *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"h_list\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"h_ch\": \"%c\",\n", item->h_ch);
        print_indent(level + 1, output);
        fprintf(output, "\"h_desc\": \"%s\"\n", item->h_desc);
        print_indent(level, output);
    }
        
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_coord(coord *item, FILE *output)
{
    print_coord_internal(item, 0, 0, output);
}

void print_coord_internal(coord *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"coord\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");        
        print_indent(level + 1, output);
        fprintf(output, "\"x\": %d,\n", item->x);
        print_indent(level + 1, output);
        fprintf(output, "\"y\": %d", item->y);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");
    
    if(embedded) {
        fprintf(output, ",");
    }
    
    fprintf(output, "\n");
}

void print_str_t(str_t *item, FILE *output)
{
    print_str_t_internal(item, 0, 0, output);
}

void print_str_t_internal(str_t *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"str_t\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"st_str\": %d,\n", item->st_str);
        print_indent(level + 1, output);
        fprintf(output, "\"st_add\": %d", item->st_add);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
};

void print_linked_list(struct linked_list *item, FILE *output)
{
    print_linked_list_internal(item, 0, 0, output);
}

void print_linked_list_internal(struct linked_list *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"linked_list\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"l_prev\": \"%p\",\n", item->l_prev);
        print_indent(level + 1, output);
        fprintf(output, "\"l_next\": \"%p\",\n", item->l_next);
        print_indent(level + 1, output);
        fprintf(output, "\"l_data\": \"%s\"", item->l_data);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_magic_item(struct magic_item *item, FILE *output)
{
    print_magic_item_internal(item, 0, 0, output);
}

void print_magic_item_internal(struct magic_item *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"magic_item\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"mi_name\": \"%s\",\n", item->mi_name);
        print_indent(level + 1, output);
        fprintf(output, "\"mi_prob\": %d,\n", item->mi_prob);
        print_indent(level + 1, output);
        fprintf(output, "\"mi_worth\": %d", item->mi_worth);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_room(struct room *item, FILE *output)
{
    print_room_internal(item, 0, 0, output);
}

void print_room_internal(struct room *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"room\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"r_pos\":\n");
        print_coord_internal(&item->r_pos, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"r_max\":\n");
        print_coord_internal(&item->r_max, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"r_gold\":\n");
        print_coord_internal(&item->r_gold, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"r_goldval\": %d,\n", item->r_goldval);
        print_indent(level + 1, output);
        fprintf(output, "\"r_flags\": %d,\n", item->r_flags);
        print_indent(level + 1, output);
        fprintf(output, "\"r_nexits\": %d", item->r_nexits);

        if(item->r_nexits != 0) {
            fprintf(output, ",");
        }

        fprintf(output, "\n");
        
        int i;
        for(i = 0; i < item->r_nexits; ++i) {
            print_indent(level + 1, output);
            fprintf(output, "\"r_exit[%d]\":\n", i);
            print_coord_internal(&item->r_exit[i], level + 1, 1, output);
        }
    }
    else {
        fprintf(output, "\n");
    }
    
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_trap(struct trap *item, FILE *output)
{
    print_trap_internal(item, 0, 0, output);
}

void print_trap_internal(struct trap *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"trap\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"tr_pos:\n");
        print_coord_internal(&item->tr_pos, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"tr_type: \"%c\",\n", item->tr_type);
        print_indent(level + 1, output);
        fprintf(output, "\"tr_flags: %d", item->tr_flags);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");
    
    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_stats(struct stats *item, FILE *output)
{
    print_stats_internal(item, 0, 0, output);
}

void print_stats_internal(struct stats *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"stats\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"s_str\":\n");
        print_str_t_internal(&item->s_str, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"s_exp\": %ld,\n", item->s_exp);
        print_indent(level + 1, output);
        fprintf(output, "\"s_lvl\": %d,\n", item->s_lvl);
        print_indent(level + 1, output);
        fprintf(output, "\"s_arm\": %d,\n", item->s_arm);
        print_indent(level + 1, output);
        fprintf(output, "\"s_hpt\": %d,\n", item->s_hpt);
        print_indent(level + 1, output);
        fprintf(output, "\"s_dmg\": \"%s\"", item->s_dmg);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_thing(struct thing *item, FILE *output)
{
    print_thing_internal(item, 0, 0, output);
}

void print_thing_internal(struct thing *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"thing\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"t_pos\":\n");
        print_coord_internal(&item->t_pos, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"t_turn\": %d\",\n", item->t_turn);
        print_indent(level + 1, output);
        fprintf(output, "\"t_type\": \"%c\",\n", item->t_type);
        print_indent(level + 1, output);
        if(item->t_disguise == 0) {
            fprintf(output, "\"t_disguise\": %d,\n", item->t_disguise);
        }
        else {
            fprintf(output, "\"t_disguise\": \"%c\",\n", item->t_disguise);
        }
        print_indent(level + 1, output);
        fprintf(output, "\"t_oldch\": \"%c\",\n", item->t_oldch);
        print_indent(level + 1, output);
        fprintf(output, "\"t_dest\":\n");
        print_coord_internal(item->t_dest, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"t_flags\": %d,\n", item->t_flags);
        print_indent(level + 1, output);
        fprintf(output, "\"t_stats\":\n");
        print_stats_internal(&item->t_stats, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"t_pack\":\n");
        print_linked_list_internal(item->t_pack, level + 1, 0, output);
    }
    else {
        fprintf(output, "\n");
    }
   
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_monster(struct monster *item, FILE *output)
{
    print_monster_internal(item, 0, 0, output);
}

void print_monster_internal(struct monster *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"monster\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"m_name\": \"%s\",\n", item->m_name);
        print_indent(level + 1, output);
        fprintf(output, "\"m_carry\": %d,\n", item->m_carry);
        print_indent(level + 1, output);
        fprintf(output, "\"m_flags\": %d,\n", item->m_flags);
        print_indent(level + 1, output);
        fprintf(output, "\"m_stats\":\n");
        print_stats_internal(&item->m_stats, level + 1, 1, output);
    }
    else {
        fprintf(output, "\n");
    }
    
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_object(struct object *item, FILE *output)
{
    print_object_internal(item, 0, 0, output);
}

void print_object_internal(struct object *item, int level, int embedded, FILE *output)
{
    print_indent(level, output);
    fprintf(output, "{\n");
    print_indent(level + 1, output);
    fprintf(output, "\"type\": \"object\",\n");
    print_indent(level + 1, output);
    fprintf(output, "\"address\": \"%p\"", item);

    if(item != NULL) {
        fprintf(output, ",\n");
        print_indent(level + 1, output);
        fprintf(output, "\"o_type\": %d,\n", item->o_type);
        print_indent(level + 1, output);
        fprintf(output, "\"o_pos\":\n");
        print_coord_internal(&item->o_pos, level + 1, 1, output);
        print_indent(level + 1, output);
        fprintf(output, "\"o_text\": \"%s\",\n", item->o_text);
        print_indent(level + 1, output);
        fprintf(output, "\"o_launch\": \"%d\",\n", item->o_launch);
        print_indent(level + 1, output);
        fprintf(output, "\"o_damage\": \"%s\",\n", item->o_damage);
        print_indent(level + 1, output);
        fprintf(output, "\"o_hurldmg\": \"%s\",\n", item->o_hurldmg);
        print_indent(level + 1, output);
        fprintf(output, "\"o_count\": %d,\n", item->o_count);
        print_indent(level + 1, output);
        fprintf(output, "\"o_which\": %d,\n", item->o_which);
        print_indent(level + 1, output);
        fprintf(output, "\"o_hplus\": %d,\n", item->o_hplus);
        print_indent(level + 1, output);
        fprintf(output, "\"o_dplus\": %d,\n", item->o_dplus);
        print_indent(level + 1, output);
        fprintf(output, "\"o_ac\": %d,\n", item->o_ac);
        print_indent(level + 1, output);
        fprintf(output, "\"o_flags\": %d,\n", item->o_flags);
        print_indent(level + 1, output);
        fprintf(output, "\"o_group\": %d", item->o_group);
    }

    fprintf(output, "\n");
    print_indent(level, output);
    fprintf(output, "}");

    if(embedded) {
        fprintf(output, ",");
    }

    fprintf(output, "\n");
}

void print_indent(int x, FILE *output)
{
    int i;
    for(i = 0; i < x; ++i) {
        fprintf(output, "\t");
    }
}
