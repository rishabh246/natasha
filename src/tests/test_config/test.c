#include <stdio.h>

#include "action_nat.h"
#include "natasha.h"


static void
dump_rules(struct app_config_node *root, int level)
{
    if (!root) {
        return ;
    }

    printf("EXPECT: %i ", level);

    switch (root->type) {
    case NOOP:      printf("NOOP"); break;
    case ACTION:    printf("ACTION"); break;
    case SEQ:       printf("SEQ"); break;
    case IF:        printf("IF"); break;
    case COND:      printf("COND"); break;
    case AND:       printf("AND"); break;
    case OR:        printf("OR"); break;

    default:
        fprintf(stderr, "Test error: unexpected node type %i\n", root->type);
        exit(1);
    }

    printf("\n");

    if (root->left) {
        dump_rules(root->left, level + 1);
    }
    if (root->right) {
        dump_rules(root->right, level + 1);
    }
}


int
main(int argc, char **argv)
{
    int ret;
    struct app_config app_config = {};
    size_t i;

    if ((ret = rte_eal_init(argc, argv)) < 0) {
        fprintf(stderr, "Error with EAL initialization\n");
        exit(1);
    }

    if (app_config_reload(&app_config, argc - ret, argv + ret, 1) < 0) {
        fprintf(stderr, "Unable to load configuration\n");
        exit(1);
    }

    // Dump ports
    for (i = 0; i < sizeof(app_config.ports) / sizeof(*app_config.ports); ++i) {
        if (app_config.ports[i].ip) {
            printf("EXPECT: port %lu = " IPv4_FMT "\n", i, IPv4_FMTARGS(app_config.ports[i].ip));
        }
    }

    // Dump NAT rules
    nat_dump_rules("EXPECT:", app_config.nat_lookup);

    dump_rules(app_config.rules, 0);

    return 0;
}
