/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int x;
    int y;
    int *r;

    
    x = 0;
    while (x < numsSize)
    {
        y = x + 1;
        while (y < numsSize)
        {
            if (nums[x] + nums[y] == target)
            {
                r = malloc(sizeof(int) * 2);
                if (!r)
                    return (NULL);
                *returnSize = 2;
                r[0] = x;
                r[1] = y;
                return (r);
            }
            y++;
        }
        x++;
    }
    return (NULL);
}