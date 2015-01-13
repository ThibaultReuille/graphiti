__kernel void repulsion(__global float4* inNodes,                               
                        __global float4* outDirections,                         
                        const unsigned long count,                              
                        const float k)                                          
{                                                                               
    unsigned int i = get_global_id(0);                                          
    float4 f = (float4)(0.0);                                                   
    for(unsigned long j = 0; j < count; j++)                                    
    {                                                                           
       if (i != j)                                                              
       {                                                                        
           float4 direction = inNodes[i] - inNodes[j];                          
           float magnitude = length(direction);                                 
           if (magnitude > 0.0)                                                 
               f += (direction / magnitude) * (k * k / magnitude);              
       }                                                                        
    }                                                                           
    outDirections[i] = f;                                                       
}                                                                               

__kernel void attraction(__global float4* inNodes,                              
                         __global ulong2* inEdges,                              
                         __global float4* outDirections,                        
                         const unsigned long count,                             
                         const float k)                                         
{                                                                               
    unsigned int i = get_global_id(0);                                          
    unsigned long src = inEdges[i].x;                                           
    unsigned long dst = inEdges[i].y;                                           
    float4 pos1 = inNodes[src];                                                 
    float4 pos2 = inNodes[dst];                                                 
    float4 direction = pos1 - pos2;                                             
    float magnitude = length(direction);                                        
    if (magnitude > 100.0)                                                      
    {                                                                           
        outDirections[src] -= direction * magnitude / k;                        
        outDirections[dst] += direction * magnitude / k;                        
    }                                                                           
}                                                                               

__kernel void movement(__global float4* inNodes,                                
                       __global float4* inForces,                               
                       __global float4* outNodes,                               
                       const float temperature)                                 
{                                                                               
     unsigned int i = get_global_id(0);                                         
     if (length(inNodes[i]) < 100000)                                           
     {                                                                          
         float magnitude = length(inForces[i]);                                 
         if (magnitude > 0.0)                                                   
           outNodes[i] = inNodes[i] + temperature * inForces[i] / magnitude;    
     }                                                                          
}                                                                               
