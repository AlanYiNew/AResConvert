<script>
import { CircleCheck, CircleCheckFilled, CircleCloseFilled } from '@element-plus/icons-vue';
import { useTransitionFallthroughEmits } from 'element-plus';


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
    data() {
        return {
            nodes: [],
            logs: [],
            progress: 0,
            selectValue: "",
            status: {},
            options: [{
                    value: 'Json',
                    label: '转换Json'
                }, {
                    value: 'Bin',
                    label: '转换Bin'
                }],
            conf: null,
            convertMethod: null,
        };
    },
    mounted() {
        let _that = this;
        this.getConf();
    },
    methods: {
        getConf() {
            this.conf = {};
            window.GetConf().then((e) => {
                if (e.code == 0) {
                    this.conf = e.conf;
                    if (e.conf.convert == 'json') {
                        this.selectValue = "转换Json";
                        this.convertMethod = window.ConvertJson;
                    }
                    else {
                        this.selectValue = "转换Bin";
                        this.convertMethod = window.Convert;
                    }
                }
            });
            this.nodes = [];
            window.GetResourceNameAll().then((e) => {
                let viewData = resourceMTV(e);
                this.nodes = viewData;
            });
        },
        renderLog(logs) {
            for (let idx in logs) {
                this.logs.push(logs[idx]);
            }
        },
        clickConvert() {
            console.log("cc");
            let nodes = this.$refs.tree.getCheckedKeys().concat(this.$refs.tree.getHalfCheckedKeys());
            if (nodes.length == 0)
                return;
            this.progress = 0;
            this.status = {};
            this.convertOne(nodes, 0);
        },
        convertOne(nodes, idx) {
            console.log(this.selectValue);
            if (idx < nodes.length) {
                let req = {
                    res_name: nodes[idx]
                };
                try {
                    this.convertMethod(req).then((e) => {
                        if (e.log) {
                            this.renderLog(e.log);
                        }
                        if (idx + 1 != nodes.length) {
                            this.progress += Math.floor(100 / this.nodes.length);
                        }
                        else {
                            this.progress = 100;
                        }
                        console.group(e)
                        if (e.code != null) {
                            this.status[nodes[idx]] = false;
                        }
                        else {
                            this.status[nodes[idx]] = true;
                        }
                        this.convertOne(nodes, idx + 1);
                    });
                }
                catch (e) {
                    this.status[nodes[idx]] = false;
                    this.renderLog(str(e));
                    this.convertOne(nodes, idx + 1);
                }
            }
        },
        uploadToCloudOne(nodes, idx) {
            if (idx < nodes.length) {
                let req = {
                    res_name: nodes[idx]
                };
                window.Upload(req).then((e) => {
                    if (e.log) {
                        this.renderLog(e.log);
                    }
                    this.uploadToCloudOne(nodes, idx + 1);
                });
            }
        },
        uploadToCloud() {
            console.log("uploadToCloud");
            let nodes = this.$refs.tree.getCheckedKeys().concat(this.$refs.tree.getHalfCheckedKeys());
            this.uploadToCloudOne(nodes, 0);
        },
        refresh() {
            let req = {};
            this.progress = 0;
            this.logs = [];
            this.nodes = [];
            this.status = {};
            window.Refresh(req).then((e) => {
                this.getConf();
                if (e.log) {
                    this.renderLog(e.log);
                }
            });
        },
        handleCheckAllChange(x) {
            if (x) {
                let all = this.nodes.map((element) => element.id);
                console.log(all);
                this.$refs.tree.setCheckedKeys(all);
            }
            else {
                this.$refs.tree.setCheckedNodes([]);
            }
        },
        getStatus(node, d) {
            const key = node.data.id;
            if (!(key in this.status)) {
                return "none";
            }
            else if (this.status[key] == false) {
                return "fail";
            }
            else {
                return "success";
            }
        }
    },
    components: { CircleCheck, CircleCheckFilled, CircleCloseFilled }
}
</script>

<template>
   <div class="common-layout">
      <el-row :gutter="10" style="height:100%">
        <el-col :xs="10" :sm="8" :md="6" :lg="6" :xl="6">
          <el-card style="height:100%">
            <el-checkbox size="mini" 
              @change="handleCheckAllChange" style="padding:0px;margin-left:26.5px;">
            All</el-checkbox>
            <el-tree ref="tree" node-key="id" show-checkbox :data="nodes" :props="defaultProps" @node-click="handleNodeClick" default-expand-all>
              <template #default="{ node}">
                <span class="custom-tree-node" >
                  <span>{{ node.label }}</span>
                  <span>

                    <el-icon style="vertical-align: middle" size="20" v-if="getStatus(node) =='success'">
                        <CircleCheckFilled class="success"/>
                    </el-icon>
                    <el-icon style="vertical-align: middle" size="20" v-else-if="getStatus(node) =='fail'">
                        <CircleCloseFilled class="fail"/>
                    </el-icon>
                  </span>
                </span>
              </template>
            </el-tree>
          </el-card>
        </el-col>
        <el-col :xs="5" :sm="5" :md="3" :lg="3" :xl="2">
          
            <el-card class="item" style="height:100%">
              <el-row>
                <el-col style="text-align:center">
                  <el-select v-model="selectValue" placeholder="请选择">
                    <el-option
                      v-for="item in options"
                      :key="item.value"
                      :label="item.label"
                      :value="item.value">
                    </el-option>
                  </el-select>
                </el-col>
              </el-row>
              <el-row style="margin-top:25px">
                <el-col style="text-align:center">
                <el-button :dark="isDark" @click="clickConvert" color="#2f7cce" style="height:42px" circle>
                  <el-icon style="vertical-align: middle" size="24">
                    <VideoPlay />
                  </el-icon>
                </el-button>
                </el-col>
              </el-row>
              <el-row style="margin-top:25px">
                <el-col style="text-align:center">
                <el-button :dark="isDark" @click="refresh" color="#2f7cce" style="height:42px" plain circle>
                  <el-icon style="vertical-align: middle" size="24">
                    <Refresh />
                  </el-icon>
                </el-button>
                </el-col>
              </el-row>
              
              <el-row style="margin-top:25px">
                <el-col style="text-align:center">
                  <el-button :dark="isDark" @click="uploadToCloud" color="#2f7cce" style="height:42px" plain circle>
                    <el-icon style="vertical-align: middle" size="24">
                      <UploadFilled />
                    </el-icon>
                  </el-button>
                </el-col>
              </el-row>
            </el-card>
        </el-col>
        <el-col style="display:flex;flex-direction:column" :xs="9" :sm="11" :md="15" :lg="15" :xl="16">
          <el-progress :percentage="progress" :text-inside="true" style="display:flex;justify-content: center;align-items: center;"></el-progress>
          <el-card style="height:100%">
            <div v-for="(log,index) in logs" :key="index">
              {{log}}
            </div>
          </el-card>
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
  .success {
    color: #67C23A;
  }
  .fail {
    color: #F56C6C;
  }
  .none {
    display: none;
  }
  .custom-tree-node {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: space-between;
    font-size: 14px;
    padding-right: 8px;
  }

  ::v-deep .el-tree-node__label{
    display: flex;
    flex:1;
  }
 
</style>
