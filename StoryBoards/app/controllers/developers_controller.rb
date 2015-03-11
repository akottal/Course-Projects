class DevelopersController < ApplicationController
  before_action :set_developer, only: [:show, :edit, :update, :destroy]

  # GET /developers
  # GET /developers.json

  def has_access(developer)
    current_user.id == developer.id
  end

  def redirect_if_not_same_user(developer)
    if developer.id != current_user.id
      dev_redirect
    end
  end

  def index
      redirect_if_not_login
      redirect_if_not_admin
      @developers = Developer.all
  end

  # GET /developers/1
  # GET /developers/1.json
  def show
    redirect_if_not_login
    @d = Developer.find(params[:id])
    redirect_if_not_same_user(@d) if !is_admin
    @project = nil
    @story   = nil
    if @d.project_id != nil
      @project = Project.find(@d.project_id)
    end
    if @d.story_id != nil
      @story = Story.find(@d.story_id)
    end
    @stories_created = Story.all.select { |s| s.developer_id == params[:id].to_i}
  end

  # GET /developers/new
  def new
    redirect_if_not_login
    redirect_if_not_admin
    @developer = Developer.new
  end

  # GET /developers/1/edit
  def edit
    redirect_if_not_login
    developer = Developer.find(params[:id])
    redirect_if_not_same_user(developer) if !is_admin
  end

  # POST /developers
  # POST /developers.json
  def create
    @developer = Developer.new(developer_params)
    respond_to do |format|
      if @developer.save
      #  UserMailer.welcome_email(@developer).deliver_now
        format.html { redirect_to admins_path, notice: 'Developer was created and an email was sent to registered developer.' }
        format.json { render :show, status: :created, location: @developer }
      else
        format.html { render :new }
        format.json { render json: @developer.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /developers/1
  # PATCH/PUT /developers/1.json
  def update
    respond_to do |format|
      if @developer.update(developer_params)
        if @isAdmin
          format.html { redirect_to admins_path, notice: 'Developer was successfully updated.' }
          format.json { render :show, status: :ok, location: @developer }
        else
          format.html { redirect_to @developer, notice: 'Developer was successfully updated.' }
          format.json { render :show, status: :ok, location: @developer }
        end
      else
        format.html { render :edit }
        format.json { render json: @developer.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /developers/1
  # DELETE /developers/1.json
  def destroy
    @developer.destroy
    respond_to do |format|
      format.html { redirect_to :back, notice: 'Developer was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_developer
      @developer = Developer.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def developer_params
      params.require(:developer).permit(:name,:email, :password, :password_confirmation,:project_id, :story_id)
    end
end