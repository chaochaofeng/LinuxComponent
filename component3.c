#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/component.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int slave_cmt_bind(struct device *dev, struct device *mdev, void *data)
{
	pr_err("%s entry\n", __func__);
	pr_err("%s master dev:%s slave dev:%s\n", __func__, mdev->of_node->name, dev->of_node->name);
	return 0;
}

static void slave_cmt_unbind(struct device *dev, struct device *mdev, void *data)
{
	pr_err("%s entry\n", __func__);
	pr_err("%s master dev:%s slave dev:%s\n", __func__, mdev->of_node->name, dev->of_node->name);
}


static const struct component_ops slave_cmt_ops = {
	.bind = slave_cmt_bind,
	.unbind = slave_cmt_unbind,
};

int slave_cmt_probe(struct platform_device *pdev)
{
	pr_err("%s entry %s\n", __func__, pdev->dev.of_node->name);

	return component_add(&pdev->dev, &slave_cmt_ops);
}
int slave_cmt_remove(struct platform_device *pdev)
{
	pr_err("%s entry\n", __func__);

	component_del(&pdev->dev, &slave_cmt_ops);
	return 0;
}

const struct of_device_id of_match_table[] = {
	{.compatible = "slave3,component",},
	{},
};

struct platform_driver slave_cmt_driver = 
{
	.probe = slave_cmt_probe,
	.remove = slave_cmt_remove,
	.driver = {
		.name = "slave3_cmt",
		.of_match_table = of_match_table,
	},
};

static int __init slave_component_init(void)
{
	pr_err("%s entry\n", __func__);
	return platform_driver_register(&slave_cmt_driver);
}


static void __exit slave_component_exit(void)
{
	platform_driver_unregister(&slave_cmt_driver);
}


module_init(slave_component_init);
module_exit(slave_component_exit);

MODULE_LICENSE("GPL");

