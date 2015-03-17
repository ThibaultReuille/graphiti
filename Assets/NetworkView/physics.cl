typedef struct
{
    float4 Direction;
} ParticleForce;

typedef struct
{
    float4 Position;
    float4 Color;
    float Size;

    ParticleForce Force;

    // float3 __Unused; // NOTE: Padding
} NodeInstance;

typedef struct
{
    float4 SourcePosition;
    float4 SourceColor;
    float4 TargetPosition;
    float4 TargetColor;
    float Width;

    unsigned int SourceID;
    unsigned int TargetID;

    // float __Unused;
} EdgeInstance;


__kernel void repulsion(__global NodeInstance* node_instances, const float k)
{
    unsigned long node_count = get_global_size(0);
    unsigned long i = get_global_id(0);

    for (unsigned long j = i + 1; j < node_count; j++)
    {
        float4 direction = node_instances[i].Position - node_instances[j].Position;
        float magnitude = length(direction.xyz); // NOTE: Make sure we ignore W component
        if (magnitude > 0.0)
        {
            float4 f = (direction / magnitude) * (k * k / magnitude);
            node_instances[i].Force.Direction += f;
            node_instances[j].Force.Direction -= f;
        }
    }
}

__kernel void attraction(__global NodeInstance* node_instances,
                         __global EdgeInstance* edge_instances,
                         const float k)
{
    unsigned int i = get_global_id(0);

    unsigned int src = edge_instances[i].SourceID;
    unsigned int dst = edge_instances[i].TargetID;

    float4 direction = node_instances[src].Position - node_instances[dst].Position;
    float magnitude = length(direction.xyz); // NOTE: Make sure we ignore W component

    node_instances[src].Force.Direction -= direction * magnitude / k;
    node_instances[dst].Force.Direction += direction * magnitude / k;
}

__kernel void node_animation(__global NodeInstance* instances, const float temperature)
{
    unsigned long i = get_global_id(0);

    float magnitude = length(instances[i].Force.Direction.xyz); // NOTE: Make sure we ignore W component
    if (magnitude > 0.0)
    {
        float4 new_pos = temperature * instances[i].Force.Direction / magnitude;

        if (length(new_pos.xyz) < 100000) // NOTE: Sphere Radius
        {
            instances[i].Position += new_pos;
        }
    }

    instances[i].Force.Direction = (float4)(0.0);
}

__kernel void edge_animation(__global NodeInstance* node_instances,
                             __global EdgeInstance* edge_instances)
{
    unsigned long i = get_global_id(0);

    unsigned int src = edge_instances[i].SourceID;
    unsigned int dst = edge_instances[i].TargetID;

    edge_instances[i].SourcePosition = node_instances[src].Position;
    edge_instances[i].TargetPosition = node_instances[dst].Position;
}
