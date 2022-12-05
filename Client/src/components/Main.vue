<script>

function resourceMTV(data) {
  let viewData = []
  console.log(data)
  for (let idx in data.result){
    let res_name = data.result[idx].res_name;
    viewData.push(
      { 
        label : res_name,
        id: res_name
      }
    );
  }
  return viewData;
}

export default {
  data(){
    return {
      nodes :  [],
      logs: []
    }
  },
  mounted() {
    let _that = this;
    window.GetErrMessage().then((e) => {
      console.log(e)
      if (e.code == 0){
        window.GetResourceNameAll().then((e) => {
          let viewData = resourceMTV(e);
          this.nodes = viewData;
        })
      }
      this.renderLog(e.log);
    });

    
    
  },
  methods: {
    renderLog(logs){
      for (let idx in logs){
        this.logs.push(logs[idx]);
      }
    },
    clickConvert() {
      console.log("cc")
      let nodes = this.$refs.tree.getCheckedKeys().concat(this.$refs.tree.getHalfCheckedKeys());    
      this.convertOne(nodes, 0)
    },
    convertOne(nodes, idx){
      console.log("aa", idx, nodes.length)
        if (idx < nodes.length){
          let req = {
            res_name:nodes[idx]
          }
          
          window.Convert(req).then((e)=>{
            if (e.code !=0){
               this.renderLog(e.log);
            }
            this.convertOne(nodes, idx+1)
          })
        }
    }
  }
}
</script>

<template>
   <div class="common-layout">
      <el-row :gutter="10" style="height:100%">
        <el-col :xs="10" :sm="8" :md="6" :lg="6" :xl="6">
          <el-card style="height:100%"><el-tree ref="tree" node-key="id" show-checkbox :data="nodes" :props="defaultProps" @node-click="handleNodeClick" />
          </el-card>
        </el-col>
        <el-col style="display:flex;flex-direction:column" :xs="14" :sm="16" :md="18" :lg="18" :xl="18">
          <el-row >
            <el-col >
              <el-card>
                <el-button color="#626aef" :dark="isDark" plain @click="clickConvert">运行</el-button>
              </el-card>
            </el-col>
          </el-row>
          <el-row style="height:100%;margin-top: 0.5rem">
            <el-col>
              <el-card style="height:100%">
                <div v-for="(log,index) in logs" :key="index">
                  {{log}}
                </div>
              </el-card>
            </el-col>
          </el-row>
        </el-col>
      </el-row>
  </div>
</template>

<style scoped>
  .el-tree:deep(){ 
    --el-tree-node-hover-bg-color: none !important;;
  }
  .common-layout{
    width: 100%;
    height: 100%;
  }
</style>
