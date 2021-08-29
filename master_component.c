#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/component.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_graph.h>


int compare_of(struct device *dev, void *data)
{
	return dev->of_node == data;
}

static int master_cmt_bind(struct device *dev)
{
	pr_err("%s entry\n", __func__);

	component_bind_all(dev, NULL);
	return 0;
}

static void master_cmt_unbind(struct device *dev)
{
	pr_err("%s entry\n", __func__);

	component_unbind_all(dev, dev->of_node);
}


static const struct component_master_ops master_cmt_ops = {
	.bind = master_cmt_bind,
	.unbind = master_cmt_unbind,
};

int master_cmt_probe(struct platform_device *pdev)
{
	struct component_match *match = NULL;
	struct device_node *np = NULL;
	struct device_node *ep = NULL, *remote = NULL;
	struct device *dev = &pdev->dev;
	int i = 0;

	pr_err("%s entry\n", __func__);

	for(i = 0; ; i++) {
		np = of_parse_phandle(dev->of_node, "ports", i);
		if (!np) {
			break;
		}

		component_match_add(dev, &match, compare_of, np);
		pr_err("%s add component:%s\n", __func__, np->name);

		for_each_endpoint_of_node(np, ep)
		{
			remote = of_graph_get_remote_port_parent(ep);
			if(!remote || !of_device_is_available(remote))
			{
				pr_err("remote device is not available\n");
				of_node_put(remote);
				continue;
			}
			else if (!of_device_is_available(remote->parent)) {
				pr_err("parent device of %s is not available\n",
					 remote->full_name);
				of_node_put(remote);
				continue;
			}

			component_match_add(dev, &match, compare_of, remote);
			pr_err("%s add component:%s\n", __func__, remote->name);

			of_node_put(remote);
		}
	}

	return component_master_add_with_match(&pdev->dev, &master_cmt_ops, match);
}

int master_cmt_remove(struct platform_device *pdev)
{
	pr_err("%s entry\n", __func__);

	component_master_del(&pdev->dev, &master_cmt_ops);

	return 0;
}

const struct of_device_id of_match_table[] = {
	{.compatible = "master,component",},
	{},
};

struct platform_driver master_cmt_driver = 
{
	.probe = master_cmt_probe,
	.remove = master_cmt_remove,
	.driver = {
		.name = "master_cmt",
		.of_match_table = of_match_table,
	},
};

static int __init master_component_init(void)
{
	pr_err("%s entry\n", __func__);
	return platform_driver_register(&master_cmt_driver);
}

static void __exit master_component_exit(void)
{
	platform_driver_unregister(&master_cmt_driver);
}

subsys_initcall(master_component_init);
module_exit(master_component_exit);

MODULE_LICENSE("GPL");

